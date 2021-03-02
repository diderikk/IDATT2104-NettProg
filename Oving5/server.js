const express = require('express')
const app = express()
const { exec } = require("child_process")
const bodyParser = require("body-parser")
const path = require('path')
const fs = require("fs");
const { stdout } = require('process')
const PORT = 3000

app.use(bodyParser.urlencoded({extended: false}));
app.use(bodyParser.json());



app.use(express.static(path.join(__dirname,'public')));

app.get("",(req, res) =>{
    res.sendFile(path.join(__dirname,'public','index.html'));
});

app.post("/code", (req, res) => {
    writeToFile(req.body.code);
    exec("docker cp main.cpp cpp:main.cpp", () => {
        exec("docker exec cpp g++ main.cpp -o main", () => {
            exec("docker exec cpp ./main", (error, stdout, stderr) => {
                res.send(JSON.stringify({compiled: stdout}));
            })
        })
    })
    
})

const server = app.listen(PORT, () => {
    console.log("Server listening on port: " + PORT);
    exec("docker run -td --rm --name cpp cpp-image");
    
});

process.on("SIGINT", () => {
    exec("docker stop cpp");
    server.close();
})

function writeToFile(code){
    fs.writeFile("main.cpp", code, (er) =>{
        if(er) throw er;
        console.log("File written");
    })
}