/// This structure describes a single pack file used as a container for 
/// game resources in the client.
pub struct Pack {
	encrypted: bool,
	name: String,
	
}

impl Pack {
	fn new(e: bool, n: String) -> Self {
		Self {
			encrypted: e,
			name: n,
		}
	}
}