#![no_std]
#![no_main]

// use arduino_hal::prelude::*;
use panic_halt as _;

// use arduino_hal::adc;

#[arduino_hal::entry]
fn main() -> ! {
    let dp = arduino_hal::Peripherals::take().unwrap();
    let pins = arduino_hal::pins!(dp);
    let mut serial = arduino_hal::default_serial!(dp, pins, 57600);
    // let mut adc = arduino_hal::Adc::new(dp.ADC, Default::default());

    // let mut button0 = pins.d2.into_pull_up_input();
    // let mut button1 = pins.d3.into_pull_up_input();
    // let mut button2 = pins.d4.into_pull_up_input();
    // let mut button3 = pins.d5.into_pull_up_input();

    let mut led0 = pins.d9.into_output();
    // let mut led1 = pins.d10.into_output();
    // let mut led2 = pins.d11.into_output();
    // let mut led3 = pins.d12.into_output();

    // #define POT0 A0
    // #define POT1 A1
    // #define POT2 A2
    // #define POT3 A3
    // #define POT4 A4
    // #define FADER_POT A5

    // let mut pot0 = pins.a0.into_analog_input(&mut adc);

    loop {
        led0.toggle();
        arduino_hal::delay_ms(1000);
        led0.toggle();
    }
}
