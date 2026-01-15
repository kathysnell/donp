/*
	Welcome to the Desciptive Object Notated Protocol (DONP) application
    	where protocol messages described in JSON files are constructed,
		then used to simulate transactions via a simulated transport
		mechanism. The message log and application statistics are gathered
		and displayed at the end of the run!

		Copyright (c) 2025 Kathy Snell, All rights reserved.

*/

// Constants
const _MODBUS_RTU: &str = "modbus_rtu";
const _MODBUS_ASCII: &str = "modbus_ascii";
const ACTIVE: &str = _MODBUS_RTU;
const _MODBUS_RTU_JSON: &str = "modbusRtu.json";
const _MODBUS_ASCII_JSON: &str = "modbusAscii.json";

mod donp {
       
    mod observe;
    mod component;
    mod element;
    mod transform;
    mod transport;
    use crate::{_MODBUS_RTU, _MODBUS_ASCII, ACTIVE, _MODBUS_RTU_JSON, _MODBUS_ASCII_JSON}; 
    use log::{info, error};
    use std::fs::File;
    use std::io::Read;
    use std::io::Write;
    use env_logger;

     /*
        The DescObjNotatedProtocol struct represents the main application
        for the DONP protocol simulation. It includes the protocol name
        and statistics tracking for the application's performance.
    */

    pub struct DescObjNotatedProtocol {
        pub protocol: component::protocol::Protocol,
        pub stats: observe::statistics::Statistics,
    }
    impl DescObjNotatedProtocol {
        pub fn new() -> Result<DescObjNotatedProtocol, String> {
            let mut donp = DescObjNotatedProtocol {
                protocol: component::protocol::Protocol::new(),
                stats: observe::statistics::Statistics::new(),
            };
            donp.stats.start();
            info!("Welcome to the DONP (Descriptive Object Notated Protocol) Application!");
            let result = donp.init_protocol();
            if result.is_err() {
                donp.stats.end();
                donp.stats.log();
                return Err(result.err().unwrap_or_default());
            }
            Ok(donp)
        }

        pub fn init_protocol(&mut self) -> Result<(), String> {
            let mut builder = env_logger::Builder::new();
            builder.filter(None, log::LevelFilter::Info);
            builder.format(|buf, record| {
                writeln!(buf,
                    "{} [{} ] - {}",
                    chrono::Local::now().format("%Y-%m-%d %H:%M:%S"),
                    record.level(),
                    record.args()
                )
            });
            builder.init();
            let json_result = self.read_from_json();
            let json_string = match json_result {
                Ok(data) => data,
                Err(e) => return Err(format!("DONP App: Failed to read protocol JSON file: {}", e)),
            };
            let map: &str = &json_string;
            let map: serde_json::Value = match serde_json::from_str(map) {
                Ok(val) => val,
                Err(e) => return Err(format!("DONP App: Failed to parse protocol JSON file: {}", e)),
            };
            if let Some(protocol) = map.get("protocol") {
                let result = match self.protocol.from_map(protocol) {
                    Ok(_) => Ok(()),
                    Err(e) => Err(format!("DONP App: Failed to initialize protocol: {}", e)),
                };
                result
            } else {
                error!("DONP App: Invalid protocol type in JSON file.");
                return Err(format!("DONP App: Invalid protocol type in JSON file."));
            }            
        }

        pub fn read_from_json(&mut self) -> Result<String, String> {
            let mut filepath = String::from("../");
            let mut filename: &str = ACTIVE;
            filename =
            match filename {
               _MODBUS_RTU => _MODBUS_RTU_JSON,
               _MODBUS_ASCII => _MODBUS_ASCII_JSON,
               _ => {
                   return Err(format!("DONP app: Unknown protocol type: {}", filename));
               },
            };
            filepath = filepath + filename;
            let mut file = match File::open(&filepath) {
                Ok(f) => f,
                Err(e) => return Err(format!("DONP App: Failed to open protocol JSON file: {}", e)),
            };
            let mut data = String::new();
            let result = match file.read_to_string(&mut data) {
                Ok(_) => Ok(data),
                Err(e) => return Err(format!("DONP App: Failed to read protocol JSON file: {}", e)),
            };
            result            
        }

        pub fn run(&mut self) {
            self.protocol.run();
            self.stats.end();
            self.stats.log();
            
        }

        pub fn log(&self) {
            if !self.protocol.is_null() {
                self.protocol.log();
            }
        }
    }
}



fn main() {
    let app_start  = donp::DescObjNotatedProtocol::new(); 
    let mut app = match app_start {
        Ok(application) => application,
        Err(e) => {
            println!("DONP App: Failed to initialize protocol: {}", e);
            return;
        }
    };
    
    app.log();
    app.run();    
}