use des::cipher::BlockDecryptMut;
use des::cipher::KeyInit;
use tokio::io::AsyncReadExt;

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
	name: [u8;20],
	size: u32,
}

pub enum Exception {
	IoError,
	ContentError,
}

impl From<std::io::Error> for Exception {
	fn from(_a: std::io::Error) -> Self {
		Exception::IoError
	}
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
	
	pub async fn load(&mut self) -> Result<(), Exception>{
		println!("Opening pack {}", self.name);
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
				println!("File size mismatch {:x} {:x} for {}", size, size2, self.name);
				return Err(Exception::ContentError);
			}
			let mut index_contents = Vec::new();
			indx.read_to_end(&mut index_contents).await?;
			if self.encrypted {
				des_decrypt("~!@#%^$<".to_string(), &mut index_contents);
			}
			let mut indx = std::io::Cursor::new(index_contents);
			println!("There are {} files", size);
			for i in 0..size {
				let offset = indx.read_u32_le().await?;
				let mut name: [u8; 20] = [0;20];
				indx.read_exact(&mut name[..]).await?;
				let size = indx.read_u32_le().await?;
				self.file_data.push(FileEntry{
					offset: offset,
					name: name,
					size: size,
					});
				if self.encrypted {
					println!(" name {} offset {} size {}", String::from_utf8_lossy(&name[..]), offset, size);
					if offset as u64 + size as u64 > content_size as u64{
						println!("Invalid entry");
					}
					else {
						println!("Valid");
					}
				}
			}
		}
		else
		{
			return Err(Exception::IoError);	//probably
		}
		Ok(())
	}
}