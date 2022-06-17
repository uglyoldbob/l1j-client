use crate::Exception;
use des::cipher::BlockDecryptMut;
use des::cipher::KeyInit;
use tokio::io::AsyncReadExt;
use tokio::io::AsyncSeekExt;

/// This structure describes a single pack file used as a container for
/// game resources in the client.
pub struct Pack {
    encrypted: bool,
    name: String,
    file_data: Vec<FileEntry>,
    contents: Option<tokio::fs::File>,
}

struct FileEntry {
    offset: u32,
    name: String,
    size: u32,
}

fn des_decrypt(key: String, data: &mut Vec<u8>) {
    let key = generic_array::GenericArray::from_slice(key[..].as_bytes());
    let mut key = des::Des::new(&key);
    for chunk in data.chunks_exact_mut(8) {
        let mut data = generic_array::GenericArray::from_mut_slice(&mut chunk[..]);
        key.decrypt_block_mut(&mut data);
    }
}

impl Pack {
    pub fn new(n: String, e: bool) -> Self {
        Self {
            encrypted: e,
            name: n,
            file_data: Vec::new(),
            contents: None,
        }
    }

    fn get_file_index(&mut self, name: String) -> Option<usize> {
        for (i, n) in self.file_data.iter().enumerate() {
            if n.name == name {
                return Some(i);
            }
        }
        None
    }

    pub async fn raw_file_contents(&mut self, name: String) -> Option<Vec<u8>> {
        let index = self.get_file_index(name.clone());
        if let Some(f) = &mut self.contents {
            if let Some(i) = index {
                let offset = self.file_data[i].offset;
                let size = self.file_data[i].size;
                if let Err(_e) = f.seek(std::io::SeekFrom::Start(offset as u64)).await {
                    return None;
                }
                let mut buffer = bytes::BytesMut::with_capacity(size as usize);

                if let Err(_e) = f.read_buf(&mut buffer).await {
                    return None;
                }
                Some(buffer.to_vec())
            } else {
                None
            }
        } else {
            None
        }
    }

    pub async fn decrypted_file_contents(&mut self, name: String) -> Option<Vec<u8>> {
        let mut file = self.raw_file_contents(name).await;
        if let Some(f) = &mut file {
            des_decrypt("~!@#%^$<".to_string(), f);
            Some(f.to_vec())
        } else {
            None
        }
    }

    pub async fn load(&mut self) -> Result<(), Exception> {
        let content = format!("{}.pak", self.name);
        let index = format!("{}.idx", self.name);
        let contents = tokio::fs::File::open(content).await?;
        self.contents = Some(contents);
        if let Some(contents) = &self.contents {
            let mut indx: tokio::fs::File = tokio::fs::File::open(index).await?;
            let size = indx.read_u32_le().await? as u64;
            let size2 = (indx.metadata().await?.len() - 4) / 28;
            let content_size = contents.metadata().await?.len() as u64;
            if size != size2 {
                println!(
                    "File size mismatch {:x} {:x} for {}",
                    size, size2, self.name
                );
                return Err(Exception::ContentError);
            }
            let mut index_contents = Vec::new();
            indx.read_to_end(&mut index_contents).await?;
            if self.encrypted {
                des_decrypt("~!@#%^$<".to_string(), &mut index_contents);
            }
            let mut indx = std::io::Cursor::new(index_contents);
            for i in 0..size {
                let offset = indx.read_u32_le().await?;
                let mut name: [u8; 20] = [0; 20];
                indx.read_exact(&mut name[..]).await?;
                let size = indx.read_u32_le().await?;
                let mut name = String::from_utf8_lossy(&name[..]).into_owned();
                name.make_ascii_lowercase();
                name = name.trim_matches(char::from(0)).to_string();
                self.file_data.push(FileEntry {
                    offset: offset,
                    name: name,
                    size: size,
                });
                if offset as u64 + size as u64 > content_size as u64 {
                    println!("Invalid entry");
                    return Err(Exception::ContentError);
                }
            }
        } else {
            return Err(Exception::IoError); //probably
        }
        Ok(())
    }
}
