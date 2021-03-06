const express = require('express')
const app = express()
const { exec } = require("child_process")
const bodyParser = require("body-parser")
const path = require('path')
const fs = require("fs");

const PORT = 3000

app.use(bodyParser.urlencoded({extended: false}));
app.use(bodyParser.json());



app.use(express.static(path.join(__dirname,'public')));

// Sends index.html on GET-request to localhost:3000
app.get("/",(req, res) =>{
    res.sendFile(path.join(__dirname,'public','index.html'));
});

/**
 * Runs posted code in a docker container
 */

app.post("/code", (req, res) => {
    // Writes a main.cpp file with the posted code
    writeToFile(req.body.code);
    // Copies the created main.cpp file into the running docker container (Runs when server starts).
    exec("docker cp main.cpp cpp:main.cpp",() => {
        // Compiles the copied main.cpp file
        exec("docker exec cpp g++ main.cpp -o main", (err,stdout,stderr) => {
            // If errror, it will be sent to the client
            if(err){
                res.send(JSON.stringify({
                    compiled: stderr
                }));
                return;
            }
            // Else it will run the compiled main.cpp program, and send stdout
            exec("docker exec cpp ./main", (err, stdout, stderr) => {
                res.send(JSON.stringify({
                    compiled: stdout
                }));
            });
        });
    })       
})

const server = app.listen(PORT, () => {
    // Checks if docker image exists on computer
    exec("docker image inspect cpp-image", (err) => {
        if(err){
            // If error, the image doesn't exist and it builds the image from Dockerfile
            console.log("Building docker image");
            exec("docker build -t cpp-image .", () => {
                console.log("Docker image built");
                // Runs a container for the recently created docker image
                exec("docker run -td --rm --name cpp cpp-image");
                console.log("Server listening on port: " + PORT);
            })
        }
        else{
            // Docker image exists and we only need to run that image and create a docker container
            exec("docker run -td --rm --name cpp cpp-image");
            console.log("Server listening on port: " + PORT);
        }
    });
});

// When program is ending
process.on("SIGINT", () => {
    // Stops the current docker container
    exec("docker stop cpp");
    // Closes server
    server.close();
})

function writeToFile(code){
    fs.writeFile("main.cpp", code, (er) =>{
        if(er) throw er;
        console.log("File written");
    })
}
