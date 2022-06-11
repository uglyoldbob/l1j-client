use sdl2::pixels::Color;
use sdl2::event::Event;
use sdl2::keyboard::Keycode;
use std::time::Duration;

use std::fs;

mod settings;

mod sprites;
use crate::sprites::*;

mod pack;
use crate::pack::*;

mod font;
use crate::font::*;

mod exception;
use crate::exception::*;

enum MessageToAsync {
	LoadResources(String),
	LoadFont(String),
}

enum MessageFromAsync {
	ResourceStatus(bool),
}

struct PackFiles {
	tile: Pack,
	text: Pack,
	sprite: Pack,
	sprites: Vec<Pack>,
}

impl PackFiles {
	pub async fn load(path: String) -> Result<Self, ()> {
		let mut packs: Vec<Pack> = Vec::new();
		for i in 0..16 {
			let mut pack = Pack::new(format!("{}/Sprite{:02}", path, i), false);
			let e = pack.load().await;
			if let Err(_a) = e {
				return Err(());
			}
			packs.push(pack);
		}
		Ok(Self {
			tile: Pack::new(format!("{}/Tile", path), false),
			text: Pack::new(format!("{}/Text", path), true),
			sprite: Pack::new(format!("{}/Sprite", path), false),
			sprites: packs,
		})
	}
}

async fn async_main(mut r: tokio::sync::mpsc::Receiver<MessageToAsync>,
	mut s: tokio::sync::mpsc::Sender<MessageFromAsync>) {
	println!("Async main");
	
	let mut resource_path: String = "".to_string();
	let mut packs: PackFiles;
	
	loop {
		let message = r.recv().await;
		match message {
			None => break,
			Some(msg) => {
				match msg {
					MessageToAsync::LoadResources(path) => {
						resource_path = path.clone();
						println!("Loading resources {}", path);
						match PackFiles::load(path).await {
							Ok(p) => {
								packs = p;
								s.send(MessageFromAsync::ResourceStatus(true)).await;
							}
							Err(()) => {
								s.send(MessageFromAsync::ResourceStatus(false)).await;
							}
						}
					}
					MessageToAsync::LoadFont(file) => {
						let path = format!("{}/{}", resource_path, file);
						let font = Font::load(path).await;
						
					}
				}
			}
		}
	}
}

pub fn main() {

    let settings_file = fs::read_to_string("./settings.ini");
    let settings_con = match settings_file {
        Ok(con) => con,
        Err(_) => "".to_string(),
    };
    let mut settings = configparser::ini::Ini::new();
    let settings_result = settings.read(settings_con);
    if let Err(e) = settings_result {
	println!("Failed to read settings {}", e);
    }

    let windowed = match settings.get("general","window").unwrap().as_str() {
        "yes" => true,
        _ => false,
    };
    
    let resources = settings.get("general","resources").unwrap();
    
    let rt = tokio::runtime::Runtime::new().unwrap();
    let (s1, r1) = tokio::sync::mpsc::channel(100);
    let (s2, mut r2) = tokio::sync::mpsc::channel(100);
    rt.spawn(async_main(r1, s2));
    
    s1.blocking_send(MessageToAsync::LoadResources(resources.clone()));
    s1.blocking_send(MessageToAsync::LoadFont("Font/eng.fnt".to_string()));
    
    println!("Loading resources from {}", resources);
    //TODO load from from Font/eng.fnt
    //TODO des key init
    //TODO load packs
    //TODO load tiles
    
    //let sprites = Sprites::load(".".to_string());

    //load program settings, including where to find resources
    let game_settings = settings::Settings { game_resources: "/nowhere".to_string() };

    //load pack files
    //Text, Tile, Sprite0-Sprite15


    let sdl_context = sdl2::init().unwrap();
    let video_subsystem = sdl_context.video().unwrap();



    let mut vid_win = video_subsystem.window("l1j-client", 640, 480);
    let mut windowb = vid_win.position_centered();

    if !windowed {
	windowb = windowb.fullscreen();
    }
    let window = windowb.build().unwrap();

    let mut canvas = window.into_canvas().build().unwrap();

    canvas.set_draw_color(Color::RGB(0, 255, 255));
    canvas.clear();
    canvas.present();
    let mut event_pump = sdl_context.event_pump().unwrap();
    let texture_creator = canvas.texture_creator();
    let mut i = 0;
    'running: loop {
	let mut fail = false;
	while let Ok(msg) = r2.try_recv() {
		match msg {
			MessageFromAsync::ResourceStatus(b) => {
				if !b {
					println!("Failed to load game resources");
					let scheme = sdl2::messagebox::MessageBoxColorScheme {
						background: (0,0,255),
						text: (0,0,0),
						button_border: (0,0,0),
						button_background: (255,255,255),
						button_selected:(0,255,255),
					};
					let mut vid_win = video_subsystem.window("l1j-client", 320, 240);
					let mut windowb = vid_win.position_centered();
					if !windowed {
						windowb = windowb.fullscreen();
					}
					let window = windowb.build().unwrap();
					sdl2::messagebox::show_message_box(
						sdl2::messagebox::MessageBoxFlag::ERROR,
						&[sdl2::messagebox::ButtonData{
							flags: sdl2::messagebox::MessageBoxButtonFlag::RETURNKEY_DEFAULT,
							button_id: 0,
							text: "OK",
						}],
						"ERROR",
						"Unable to load game resources",
						&window,
						scheme,
						);
					fail = true;
				}
			}
		}
	}
	if fail {
		break
	};
        i = (i + 1) % 255;
        canvas.set_draw_color(Color::RGB(i, 64, 255 - i));
        canvas.clear();
        for event in event_pump.poll_iter() {
            match event {
                Event::Quit { .. } |
                Event::KeyDown { keycode: Some(Keycode::Escape), .. } => break 'running,
                _ => {}
            }
        }
        // The rest of the game loop goes here...

        canvas.present();
        ::std::thread::sleep(Duration::new(0, 1_000_000_000u32 / 20));
    }
}
