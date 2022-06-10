use std::fs;
use std::collections::HashMap;

pub struct Sprites {
	sprite_defs: HashMap<u16,Sprite>,
}

pub struct SpriteActionFrame {
	sprite: u16,
	subsprite: u16,
	subframe: u16,
	length: u16,
	audio: u16,
}

pub struct Sprite {
	actions: [Vec<SpriteActionFrame>; 72],
	mspf: u16,
	shadow: u16,
	sprite_type: u16,
	attr: u16,
	num_clothing: u16,
	clothing: Vec<u16>,
}

impl Sprites {
	pub fn load(s: String) -> Self{
		let path = format!("{}/sprite_table.txt", s);
		println!("Load from {}", path);
		let mut file = fs::read(path).unwrap();
		for mut b in &mut file {
			if *b == (0xd as u8) {
				*b = ' ' as u8;
			}
		}
		let sprites: HashMap<u16,Sprite> = HashMap::new();
		for mut i in 1..file.len() {
			unimplemented!();
		}
		
		Sprites {
			sprite_defs: sprites,
		}
	}
}