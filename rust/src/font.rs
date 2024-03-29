use crate::Exception;
use sdl2::pixels::PixelFormatEnum;
use sdl2::surface::Surface;
use tokio::io::AsyncReadExt;

pub struct Font {}

impl Font {
    pub async fn load(path: String) -> Result<Self, Exception> {
        println!("Loading font {}", path);

        let mut file = tokio::fs::File::open(path).await;
        if let Err(_e) = file {
            return Err(Exception::IoError);
        }
        let mut file = file.unwrap();
        if file.metadata().await?.len() as u64 != 1140 {
            return Err(Exception::ContentError);
        }

        for character in 0..95 {
            for row in 0..12 {
                let data = file.read_u8();
            }
        }
        Ok(Self {})
    }
}
