window.onload = () =>{
    document.getElementById("submit").addEventListener("click", (event) => {
      // Textarea for the output
        let consoleHTML = document.getElementById("console");
        // Prevents reloading page
        event.preventDefault();
        // Code from the input
        codeText = document.getElementById("code").value;

        consoleHTML.value = "Compiling and running main:\n";

        // Post the code to server.js
        fetch("/code",{
          method: 'POST',
          headers: {
            "Content-Type": "application/json"
          },
          body: JSON.stringify({code: codeText})
        })
        .then(response => response.json())
        .then(data => {
          let result = data;
          console.log(data)
          consoleHTML.value += result.compiled;
        })
    });

    // Makes it possible to tab inside the input textarea
    document.getElementById('code').addEventListener('keydown', function(e) {
        if (e.key == 'Tab') {
          e.preventDefault();
          var start = this.selectionStart;
          var end = this.selectionEnd;
      
          this.value = this.value.substring(0, start) +
            "\t" + this.value.substring(end);
      
          this.selectionStart =
            this.selectionEnd = start + 1;
        }
      });
}


// Same as above, but with XHR and JQuery
// $(document).ready(() =>{
//     $("form").submit((event) => {
//         let elem = $(event.currentTarget);
//         event.preventDefault();
//         string = $("#code").val();
//         let myObj = {
//             code: string
//         }

//         const xhr = new XMLHttpRequest();
        // xhr.open("POST","/code",true);
        // xhr.setRequestHeader("Content-Type", "application/json");
        // xhr.onreadystatechange = () => {
        //     if(xhr.readyState == XMLHttpRequest.DONE){
        //         // console.log(xhr.responseText);
        //         let result = JSON.parse(xhr.responseText);
        //         $("#console").val(result.compiled);
        //     }
        // }
//         console.log(JSON.stringify(myObj));
//         xhr.send(JSON.stringify(myObj));

        
        
//     });

// })