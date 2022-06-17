use sdl2::event::Event;
use sdl2::keyboard::Keycode;
use sdl2::pixels::Color;
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

mod mode;
use crate::mode::*;

mod resources;
use crate::resources::*;

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

    let windowed = match settings.get("general", "window").unwrap().as_str() {
        "yes" => true,
        _ => false,
    };

    let resources = settings.get("general", "resources").unwrap();

    let rt = tokio::runtime::Runtime::new().unwrap();
    let (s1, r1) = tokio::sync::mpsc::channel(100);
    let (s2, mut r2) = tokio::sync::mpsc::channel(100);
    rt.spawn(async_main(r1, s2));

    let mut mode: Box<dyn GameMode> = Box::new(ExplorerMenu::new());

    s1.blocking_send(MessageToAsync::LoadResources(resources.clone()));
    s1.blocking_send(MessageToAsync::LoadFont("Font/eng.fnt".to_string()));
    s1.blocking_send(MessageToAsync::LoadSpriteTable);
    //load Font/SMALL.FNT

    println!("Loading resources from {}", resources);
    //TODO load from from Font/eng.fnt
    //TODO des key init
    //TODO load packs
    //TODO load tiles

    //let sprites = Sprites::load(".".to_string());

    //load program settings, including where to find resources
    let game_settings = settings::Settings {
        game_resources: "/nowhere".to_string(),
    };

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

    canvas.set_draw_color(Color::RGB(0, 0, 0));
    canvas.clear();
    canvas.present();
    let mut event_pump = sdl_context.event_pump().unwrap();
    let texture_creator = canvas.texture_creator();
    'running: loop {
        while let Ok(msg) = r2.try_recv() {
            match &msg {
                MessageFromAsync::ResourceStatus(b) => {
                    if !b {
                        println!("Failed to load game resources");
                        let scheme = sdl2::messagebox::MessageBoxColorScheme {
                            background: (0, 0, 255),
                            text: (0, 0, 0),
                            button_border: (0, 0, 0),
                            button_background: (255, 255, 255),
                            button_selected: (0, 255, 255),
                        };
                        let mut vid_win = video_subsystem.window("l1j-client", 320, 240);
                        let mut windowb = vid_win.position_centered();
                        if !windowed {
                            windowb = windowb.fullscreen();
                        }
                        let window = windowb.build().unwrap();
                        let _e = sdl2::messagebox::show_message_box(
                            sdl2::messagebox::MessageBoxFlag::ERROR,
                            &[sdl2::messagebox::ButtonData {
                                flags: sdl2::messagebox::MessageBoxButtonFlag::RETURNKEY_DEFAULT,
                                button_id: 0,
                                text: "OK",
                            }],
                            "ERROR",
                            "Unable to load game resources",
                            &window,
                            scheme,
                        );
                        break 'running;
                    }
                }
                MessageFromAsync::StringTable(name, data) => {
                    println!("Loaded string table {}", name);
                }
                MessageFromAsync::Png(_name, _data) => {}
            }
            mode.parse_message(&msg);
        }
        for event in event_pump.poll_iter() {
            match event {
                Event::Quit { .. }
                | Event::KeyDown {
                    keycode: Some(Keycode::Escape),
                    ..
                } => break 'running,
                _ => {}
            }
            mode.parse_event(event);
        }
        mode.draw(&mut canvas);
        canvas.present();
        let framerate = mode.framerate() as u64;
        ::std::thread::sleep(Duration::from_nanos(1_000_000_000u64 / framerate));
    }
}
