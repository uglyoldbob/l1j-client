use crate::MessageFromAsync;

/// This trait is used to determine what mode of operation the program is in
pub trait GameMode {
    fn parse_message(&mut self, m: MessageFromAsync);
    fn parse_event(&mut self, e: sdl2::event::Event);
    fn draw(&mut self, canvas: &mut sdl2::render::WindowCanvas);
    /// Framerate is specified in frames per second
    fn framerate(&self) -> u8;
}

/// This is for exploring the resources of the game client
pub struct ExplorerMenu {}

impl ExplorerMenu {
    pub fn new() -> Self {
        Self {}
    }
}

impl GameMode for ExplorerMenu {
    fn parse_message(&mut self, m: MessageFromAsync) {
        match m {
            MessageFromAsync::ResourceStatus(b) => {}
        }
    }

    fn parse_event(&mut self, e: sdl2::event::Event) {
        match e {
            _ => {}
        }
    }

    fn draw(&mut self, canvas: &mut sdl2::render::WindowCanvas) {}

    fn framerate(&self) -> u8 {
        20
    }
}
