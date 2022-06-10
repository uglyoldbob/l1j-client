use sdl2::pixels::Color;
use sdl2::event::Event;
use sdl2::keyboard::Keycode;
use std::time::Duration;

use std::fs;

mod settings;
mod sprites;

use crate::sprites::*;

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
    
    let resources = settings.get("general","resources").unwrap().to_string();
    println!("Loading resources from {}", resources);
    //TODO load from from Font/eng.fnt
    //TODO des key init
    //TODO load packs
    //TODO load tiles
    
    //let sprites = Sprites::load(".".to_string());

    let sdl_context = sdl2::init().unwrap();
    let video_subsystem = sdl_context.video().unwrap();



    let mut vid_win = video_subsystem.window("l1j-client", 650, 480);
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
    let mut i = 0;
    'running: loop {
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

        //load program settings, including where to find resources
        let game_settings = settings::Settings { game_resources: "/nowhere".to_string() };

        //load pack files
        //Text, Tile, Sprite0-Sprite15

        canvas.present();
        ::std::thread::sleep(Duration::new(0, 1_000_000_000u32 / 20));
    }
}
