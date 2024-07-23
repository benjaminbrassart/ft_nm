#![no_main]

#[no_mangle]
extern "C" fn say_hello() {}

#[no_mangle]
extern fn hello_there() {}
