use crate::MessageFromAsync;
use sdl2::pixels::Color;

/// This trait is used to determine what mode of operation the program is in
pub trait GameMode {
    fn parse_message(&mut self, m: &MessageFromAsync);
    fn parse_event(&mut self, e: sdl2::event::Event);
    fn draw(&mut self, canvas: &mut sdl2::render::WindowCanvas);
    /// Framerate is specified in frames per second
    fn framerate(&self) -> u8;
}

/// This is for exploring the resources of the game client
pub struct ExplorerMenu {
    i: u8,
}

impl ExplorerMenu {
    pub fn new() -> Self {
        Self { i: 0 }
    }
}

impl GameMode for ExplorerMenu {
    fn parse_message(&mut self, m: &MessageFromAsync) {
        match m {
            MessageFromAsync::ResourceStatus(b) => {}
            MessageFromAsync::StringTable(name, data) => {}
        }
    }

    fn parse_event(&mut self, e: sdl2::event::Event) {
        match e {
            _ => {}
        }
    }

    fn draw(&mut self, canvas: &mut sdl2::render::WindowCanvas) {
        canvas.set_draw_color(Color::RGB(self.i, 64, 255 - self.i));
        self.i = self.i.wrapping_add(1);
        canvas.clear();
    }

    fn framerate(&self) -> u8 {
        20
    }
}
