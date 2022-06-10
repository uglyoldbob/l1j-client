/// This structure describes a single pack file used as a container for 
/// game resources in the client.
pub struct Pack {
	encrypted: bool,
	name: String,
	
}

pub enum Exception {
	IoError,
}

impl From<std::io::Error> for Exception {
	fn from(_a: std::io::Error) -> Self {
		Exception::IoError
	}
}

impl Pack {
	pub fn new(n: String, e: bool) -> Self {
		Self {
			encrypted: e,
			name: n,
		}
	}
	
	pub async fn load(&mut self) -> Result<(), Exception>{
		println!("Opening pack {}", self.name);
		let content = format!("{}.pak", self.name);
		let index = format!("{}.idx", self.name);
		let cont: tokio::fs::File = tokio::fs::File::open(content).await?;
		let indx: tokio::fs::File = tokio::fs::File::open(index).await?;
		Ok(())
	}
}