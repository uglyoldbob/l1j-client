use crate::Font;
use crate::Pack;

pub mod stringtable;
use crate::resources::stringtable::*;

pub enum MessageToAsync {
    LoadResources(String),
    LoadFont(String),
    LoadSpriteTable,
    LoadTable(String),
}

pub enum MessageFromAsync {
    ResourceStatus(bool),
    StringTable(String, StringTable),
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
        let mut tile = Pack::new(format!("{}/Tile", path), false);
        tile.load().await;
        let mut text = Pack::new(format!("{}/Text", path), true);
        text.load().await;
        let mut sprite = Pack::new(format!("{}/Sprite", path), false);
        sprite.load().await;
        Ok(Self {
            tile: tile,
            text: text,
            sprite: sprite,
            sprites: packs,
        })
    }
}

pub struct Resources {
    packs: Option<PackFiles>,
}

impl Resources {
    pub fn new() -> Self {
        Self { packs: None }
    }
}

pub async fn async_main(
    mut r: tokio::sync::mpsc::Receiver<MessageToAsync>,
    mut s: tokio::sync::mpsc::Sender<MessageFromAsync>,
) {
    println!("Async main");

    let mut resource_path: String = "".to_string();
    let mut res: Resources = Resources::new();

    loop {
        let message = r.recv().await;
        match message {
            None => break,
            Some(msg) => match msg {
                MessageToAsync::LoadResources(path) => {
                    resource_path = path.clone();
                    println!("Loading resources {}", path);
                    match PackFiles::load(path).await {
                        Ok(p) => {
                            res.packs = Some(p);
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
                MessageToAsync::LoadSpriteTable => {
                    let data = include_bytes!("sprite_table.txt");
                }
                MessageToAsync::LoadTable(name) => {
                    if let Some(p) = &mut res.packs {
                        let data = p.text.decrypted_file_contents(name.clone()).await;
                        match data {
                            Some(d) => {
                                let table = StringTable::from(d);
                                s.send(MessageFromAsync::StringTable(name, table.clone()))
                                    .await;
                            }
                            None => {
                                println!("{} failed to load", name);
                            }
                        }
                    }
                }
            },
        }
    }
}
