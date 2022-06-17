#[derive(Clone)]
pub struct StringTable {
    data: Vec<String>,
}

impl StringTable {
    pub fn from(data: Vec<u8>) -> Self {
        let ascii = std::str::from_utf8(&data).unwrap();
        let lines: Vec<&str> = ascii.split("\r\n").collect();
        let count = lines[0].parse::<u32>().unwrap();
        let mut strings = Vec::with_capacity(count as usize);
        println!("There are {} entries", count);
        for l in lines.iter().skip(1) {
            strings.push(l.to_owned().to_string());
        }
        Self { data: strings }
    }
}
