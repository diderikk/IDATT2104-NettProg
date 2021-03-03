window.onload = () =>{

    document.getElementById("submit").addEventListener("click", (event) => {
        const xhr = new XMLHttpRequest();
        let consoleHTML = document.getElementById("console");
        event.preventDefault();
        codeText = document.getElementById("code").value;

        consoleHTML.value = "Compiling and running main:\n";

        xhr.open("POST","/code",true);
        xhr.setRequestHeader("Content-Type", "application/json");
        xhr.onreadystatechange = () => {
            if(xhr.readyState == XMLHttpRequest.DONE){
                // console.log(xhr.responseText);
                let result = JSON.parse(xhr.responseText);
                consoleHTML.value += result.compiled;
                console.log(result.error);
                console.log(result.stderror);
            }
        }
        xhr.send(JSON.stringify({code: codeText}));

    });

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
// Jquery without tab indenting
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