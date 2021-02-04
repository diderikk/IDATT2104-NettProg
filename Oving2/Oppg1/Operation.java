public class Operation {
    public static int doOperation(int num1, int num2, String operation){
        if(operation.toLowerCase().equals("add")){
            return num1 + num2;
        }
        else{
            return num1 - num2;
        }
        
    }
}
