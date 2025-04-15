import Foundation

import SwiftUI

import CoreBluetooth

 

    struct BluetoothDevicesView: View {

        //declares BLE

        @StateObject var bleManager = BLEManager()

        

        //title

        var body: some View {

            

            //creates a navView that allows the user to go to a diff screen that you can control BT devices from

            NavigationView{

                VStack(spacing: 10) {

                    Text("Bluetooth Devices")

                        .font(.largeTitle)

                        .frame(maxWidth: .infinity, alignment: .center)

                    

                    //makes a list of all of the discovered BT devices

                    List(bleManager.peripherals) { peripheral in

                        HStack {

                            Text(peripheral.name)

                            Spacer()

                            Text(String(peripheral.rssi))

                            Button(action: {

                                bleManager.connect(to: peripheral)

                            }) {

                                if bleManager.connectedPeripheralUUID == peripheral.id {

                                    Text("Connected")

                                        .foregroundColor(.green)

                                } else {

                                    Text("Connect")

                                }

                            }

                        }

                    }

                    .frame(height: UIScreen.main.bounds.height / 2)

                    

                    Spacer()

                    

                    //status of whether the bluetooth is on or not

                    Text("STATUS")

                        .font(.headline)

                    

                    if bleManager.isSwitchedOn{

                        Text("Bluetooth is switched on")

                            .foregroundColor(.green)

                    } else {

                        Text("Bluetooth is not switched on")

                            .foregroundColor(.red)

                    }

                    

                    Spacer()

                    

                    //buttons to start / stop scanning for devices

                    VStack(spacing: 25) {

                        Button(action: {

                            bleManager.startScanning()

                        }) {

                            Text("Start Scanning")

                        }.buttonStyle(BorderedProminentButtonStyle())

                        

                        Button(action: {

                            bleManager.stopScanning()

                        }) {

                            Text("Stop Scanning")

                        }.buttonStyle(BorderedProminentButtonStyle())

                        //navigates to other screen

                        VStack {

                            NavigationLink(destination: BTcontrol()){

                                Text("Bluetooth Control")

                            }

                        }

                    }

                    .padding()

                    

                    Spacer()

                }

                .onAppear {

                    if bleManager.isSwitchedOn{

                        bleManager.startScanning()

                    }

                }

            }

        }

    }



//NEXT


import Foundation

import Foundation

import SwiftUI

import CoreBluetooth

 

//Bluetooth class

class BLEManager: NSObject, ObservableObject, CBCentralManagerDelegate, CBPeripheralDelegate {

    var myCentral: CBCentralManager!

   

    //tracks if BT is on

    @Published var isSwitchedOn = true

    

    //array to store the IDs

    @Published var peripherals = [Peripheral]()

    

    //stores the ID of the peripheral

    @Published var connectedPeripheralUUID: UUID?

    

    //initializes the central

    override init(){

        super.init()

        myCentral = CBCentralManager(delegate: self, queue: nil)

    }

    

    //checks when BT is on

    func centralManagerDidUpdateState(_ central: CBCentralManager) {

        isSwitchedOn = central.state == .poweredOn

        //scans or does not scan depending on if BT is on

        if isSwitchedOn {

            startScanning()

        } else{

            stopScanning()

        }

    }

    

    //when a peripheral is found, IDs it, sets the ID and gets the RSSI, adds to the list

    func centralManager(_ central: CBCentralManager, didDiscover peripheral: CBPeripheral, advertisementData: [String: Any], rssi RSSI: NSNumber){

        let newPeripheral = Peripheral(id: peripheral.identifier, name: peripheral.name ?? "Unknown", rssi: RSSI.intValue)

        if !peripherals.contains(where: {$0.id == newPeripheral.id}){

            DispatchQueue.main.async {

                self.peripherals.append(newPeripheral)

            }

        }

    }

    

    //scans

    func startScanning(){

        print("startScanning")

        myCentral.scanForPeripherals(withServices: nil, options: nil)

    }

    

    //stops scan

    func stopScanning(){

        print("stopScanning")

        myCentral.stopScan()

    }

    

    //connects to a peripheral

    func connect(to peripheral: Peripheral){

        guard let cbPeripheral = myCentral.retrievePeripherals(withIdentifiers: [peripheral.id]).first else{

            print("Peripheral not found for connection")

            return

        }

        

        //connects it

        connectedPeripheralUUID = cbPeripheral.identifier

        cbPeripheral.delegate = self

        myCentral.connect(cbPeripheral, options: nil)

    }

    

    //declares that it has been connected

    func centralManager(_ central: CBCentralManager, didConnect peripheral: CBPeripheral) {

        print("Connected to \(peripheral.name ?? "Unknown")")

        peripheral.discoverServices(nil)

    }

    

    //declares failure of connection

    func centralManager(_ central: CBCentralManager, didFailToConnect peripheral: CBPeripheral, error: Error?){

        print("Failed to connect to \(peripheral.name ?? "Unknown"): \(error?.localizedDescription ?? "No error information")")

        if peripheral.identifier == connectedPeripheralUUID{

            connectedPeripheralUUID = nil

        }

    }

    

    //disconnects peripheral

    func centralManager(_ central: CBCentralManager, didDisconnectPeripheral peripheral: CBPeripheral, error: Error?){

        print("Disconnected from \(peripheral.name ?? "Unknown")")

        if peripheral.identifier == connectedPeripheralUUID{

            connectedPeripheralUUID = nil

        }

    }

    

    //looks at services

    func peripheral(_ peripheral: CBPeripheral, didDiscoverServices error: Error?){

        if let services = peripheral.services{

            for service in services{

                print("Discovered service: \(service.uuid)")

                peripheral.discoverCharacteristics(nil, for: service)

            }

        }

    }

    

    //looks at characterstics of the services

    func peripheral(_ peripheral: CBPeripheral, didDiscoverCharacteristicsFor service: CBService, error: Error?){

        if let characteristics = service.characteristics{

            for characteristic in characteristics{

                print("Discovered characteristic: \(characteristic.uuid)")

            }

        }

        

    }

    

    //sends text over

    /*func sendText(_ text: String) {

            /*guard let peripheral = peripheral,

                  let characteristic = peripheral.services?.first?.characteristics?.first(where: { $0.uuid == CHARACTERISTIC_UUID })

            else {

                print("Peripheral or characteristic not found")

                return

            }

             print("Sending: \(text)")

             let data = text.data(using: .utf8)!

             peripheral.writeValue(data, for: characteristic, type: .withoutResponse)

             */

            

            CBPeripheral.func writeValue(_ data: Data, for characteristic: CBCharacteristic, type: CBCharacteristicWriteType)

            

        }*/

}



//NEXT




import Foundation

import SwiftUI

 

struct BTcontrol: View{

    //variables

    @State var tempInput : String = "0"

    @State var presInput : String = "0"

    @State var infoText: String = "The current temp level is 0 and the pressure is 0%."

    

   //shows updated values

   func checking(){

        infoText = "The current temperature level is \(tempInput) and the pressure is at \(presInput)%."

    }

    

    //has places to enter temp / pressure / submit

    var body: some View{

        VStack (spacing: 25){

            Text(infoText)

                .padding()

            TextField("Enter a temperature level here:", text: $tempInput)

                .padding()

                .keyboardType(.decimalPad)

            TextField("Enter a pressure percentage here:", text: $presInput)

                .padding()

                .keyboardType(.decimalPad)

            Button(action: {

                checking()

            }){

                Text("Submit")

            }.buttonStyle(BorderedProminentButtonStyle())

        }

    }

}


//NEXT


//

//  SmartSlingV2App.swift

//  SmartSlingV2

//

//  Created by Gabrielle Martin on 4/4/25.

//

 

import SwiftUI

 

//main

@main

struct SmartSlingV2App: App {

    var body: some Scene {

        WindowGroup {

            BluetoothDevicesView()

        }

    }

}

//NEXT


import Foundation

 

//peripheral, has ID, name, and strength

struct Peripheral: Identifiable {

    let id: UUID

    let name: String

    let rssi: Int

}
