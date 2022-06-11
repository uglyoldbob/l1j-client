pub enum Exception {
    IoError,
    ContentError,
}

impl From<std::io::Error> for Exception {
    fn from(_a: std::io::Error) -> Self {
        Exception::IoError
    }
}
