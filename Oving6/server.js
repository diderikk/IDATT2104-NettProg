const net = require('net');
const fs = require('fs');
const crypto = require("crypto");

const HTTPPORT  = 3000;
const WSPORT = 3001;

/**
 * NOT FINISHED
 */
// TODO write to all clients, send with byte length === 126, add to index.html file for interaction, add CSS
const httpServer = net.createServer((connection) => {
    connection.on('data', () => {
        try{
            fs.readFile('index.html', (err, data) => {
                if(err){
                    throw err;
                }
                connection.write('HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: ' + data.length + '\r\n\r\n' + data);

            });
        }catch(error){
            console.log(error);
        }
        
    });
});

httpServer.listen(HTTPPORT, () => {
    console.log("HTTP server listening on port: " + HTTPPORT);
});

httpServer.on("error", (error) =>{
    console.log(error);
});

const wsServer = net.createServer((connection) => {
    console.log("Client connected");

    connection.on('data', (data) => {
        string = data.toString();
        //Finds Connection header line
        if((/GET \/ HTTP\//i).test(string)){
            console.log("Data received from client: ", string);
            if(!checkHeaderFields(string)){
                connection.write("HTTP/1.1 400 Bad Request\r\n");
                connection.end();
            }
            let key = string.match(/Sec-WebSocket-Key: (.+)?\s/i)[1].toString();
            let acceptKey = createAcceptKey(key);

            connection.write(`HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\n`+
            `Connection: Upgrade\r\nSec-WebSocket-Accept: ${acceptKey}\r\nSec-WebSocket-Protocol: JSON\r\n\r\n`);
        }
        else{
            for(let i = 0; i< data.length; i++)console.log(data[i].toString(2));
            parseData(data);
            let response = "Hello back";
            let buf = createMessage(response);
            connection.write(buf);
        }
    });

    connection.on('end', () => {
        console.log("Client disconnected");
    });
});

wsServer.on("error", (error) => {
    console.log("Error: ", error);
});

wsServer.listen(WSPORT, () => {
    console.log('Websocket server listening on port: ', WSPORT);
})



function createAcceptKey(clientKey){
    acceptKey = clientKey + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    acceptKey = crypto.createHash("sha1").update(acceptKey,"binary").digest("base64");
    return acceptKey;
}

function createMessage(text){
    // TODO handle opcode for closing. Current can only send text frame.
    // TODO masking
    let textByteLength = Buffer.from(text).length;
    if(textByteLength > 125) throw Error("Message too long");
    let secondByte = (1 << 7) | textByteLength;

    
    let maskBytes = [];
    for(let i = 0; i< 4; i++){
        maskBytes.push(Math.floor((Math.random()*125)+1))
    }
    
    const buffer1 = Buffer.from([0b10000001,secondByte]);
    const buffer2 = Buffer.from(maskBytes);
    const buffer3 = Buffer.from(text);

    for(let i = 0; i<buffer3.length; i++){
        let byte = buffer3[i] ^ buffer2[i % 4];
        buffer3[i] = byte;
    }

    const buffer = Buffer.concat([buffer1,buffer2, buffer3]);
    return buffer;

}

function parseData(data){
    //Checks if the most significant bit is 1 or 0
    //TODO handle client closing the connection
    if(data[0] & 0b1111 === 0x8) return;
    if(data[1]>>7 === 1){
        let length, maskStart;
        length = data[1] & 0b1111111;
        if(data.length < 126){
            // The least 7 significant bits of the second byte
            maskStart = 2;
            
        }
        else if(data.length === 126){
            length += ((data[2] << 8)| data[3]);
            maskStart = 4;
        }
        else if(data.length === 127){
            let temp = data[2];
            for(let i = 3; i<10;i++){
                temp = (temp << 8)|data[i];
            }
            length += temp;
            maskStart = 10;
        }
        let dataStart = maskStart + 4;


        let result = "";
        for(let i = dataStart; i< dataStart+length; i++){
            // XORs the payload with the mask bytes.
            let byte = data[i] ^ data[maskStart + ((i - dataStart) % 4)]
            result += String.fromCharCode(byte);
        }
        console.log(result);
    }
    else{

        console.log(data.toString());
    }
}


function checkHeaderFields(headers){
    let connectionReg = /Connection:.+Upgrade.*?\s/i
    let hostReg = /Host:/i
    let upgradeReg = /Upgrade:.+websocket.*?\s/i
    let keyReg = /Sec-WebSocket-Key:/i
    let versionReg = /Sec-WebSocket-Version: 13\s/i

    if(connectionReg.test(headers) && hostReg.test(headers) && upgradeReg.test(headers) && keyReg.test(headers) && versionReg.test(headers))
        return true;

    return false;
}