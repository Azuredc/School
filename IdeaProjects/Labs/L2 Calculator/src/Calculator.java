import java.util.Scanner;

public class Calculator {
    public static void main(String [] args) {

        Scanner scnr = new Scanner(System.in);
        double firstOperand;
        double secondOperand;
        int userInput;

        System.out.print("Enter first operand: "); /*First user input*/
        firstOperand = scnr.nextDouble();
        System.out.print("Enter second operand: "); /*Second user input*/
        secondOperand = scnr.nextDouble();
        System.out.println(" ");

        System.out.println("Calculator Menu"); /*Set up the Calculator Menu*/
        System.out.println("---------------");
        System.out.println("1. Addition");
        System.out.println("2. Subtraction");
        System.out.println("3. Multiplication");
        System.out.println("4. Division");
        System.out.println(" ");

        System.out.println("Which operation do you want to perform? "); /*Set up if-else statements for the different outputs*/

        userInput = scnr.nextInt();

        if (userInput == 1){
            System.out.print("The result of the operation is " + (firstOperand + secondOperand) + ". Goodbye!");
        }
        else if (userInput == 2){
            System.out.print("The result of the operation is " + (firstOperand - secondOperand) + ". Goodbye!");
        }
        else if (userInput == 3){
            System.out.print("The result of the operation is " + (firstOperand * secondOperand) + ". Goodbye!");
        }
        else if (userInput == 4){
            System.out.print("The result of the operation is " + (firstOperand / secondOperand) + ". Goodbye!");
        }
        else if ((userInput < 0) || (userInput > 4)){
            System.out.print("Error: Invalid selection! Terminating program."); /*Outlier values*/
        }
    }
}
