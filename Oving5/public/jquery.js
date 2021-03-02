$(document).ready(() =>{
    console.log($("h1").html());
    $("form").submit((event) => {
        let elem = $(event.currentTarget);
        event.preventDefault();
        string = $("#code").val();
        let myObj = {
            code: string
        }

        const xhr = new XMLHttpRequest();
        xhr.open("POST","/code",true);
        xhr.setRequestHeader("Content-Type", "application/json");
        xhr.onreadystatechange = () => {
            if(xhr.readyState == XMLHttpRequest.DONE){
                // console.log(xhr.responseText);
                let result = JSON.parse(xhr.responseText);
                $("#console").val(result.compiled);
            }
        }
        console.log(JSON.stringify(myObj));
        xhr.send(JSON.stringify(myObj));

        
        
    });

})


