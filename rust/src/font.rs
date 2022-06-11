use crate::Exception;
use sdl2::pixels::PixelFormatEnum;
use sdl2::surface::Surface;

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

        let surf = Surface::new(6, 12 * 95, PixelFormatEnum::RGB24).unwrap();

        Ok(Self {})
    }
}
