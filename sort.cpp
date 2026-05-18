#include <iostream>
#include <stack>
#include <string>
#include <vector>
#include <cmath>

int getPriority(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    if (op == '^') return 3;
    return 0;
}

bool isOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '^';
}

bool isOperand(char c) {
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}


std::vector<std::string> toPostfix(const std::string& infix) {
    std::stack<char> operators;
    std::vector<std::string> output;
    
    for (size_t i = 0; i < infix.size(); ++i) {
        char c = infix[i];
        if (c == ' ') continue;
        
        if (isOperand(c)) {
            
            std::string operand;
            while (i < infix.size() && isOperand(infix[i])) {
                if (isalpha(infix[i])) {
                    output.clear();
                    output.push_back("ERROR_LETTER");
                    return output;
                }
                operand += infix[i];
                i++;
            }
            i--;
            output.push_back(operand);
        }
        else if (c == '(') {
            operators.push(c);
        }
        else if (c == ')') {
            while (!operators.empty() && operators.top() != '(') {
                output.push_back(std::string(1, operators.top()));
                operators.pop();
            }
            if (!operators.empty()) operators.pop();
        }
        else if (isOperator(c)) {
            while (!operators.empty() && operators.top() != '(' &&
                   getPriority(operators.top()) >= getPriority(c)) {
                output.push_back(std::string(1, operators.top()));
                operators.pop();
            }
            operators.push(c);
        }
    }
    
    while (!operators.empty()) {
        output.push_back(std::string(1, operators.top()));
        operators.pop();
    }
    
    return output;
}

double calculate(const std::vector<std::string>& postfix) {
    // Проверка на ошибку с буквой
    if (!postfix.empty() && postfix[0] == "ERROR_LETTER") {
        std::cout << "ошибка: Нельзя использовать буквы" << std::endl;
        return 0;
    }
    
    std::stack<double> st;
    
    for (const auto& token : postfix) {
        if (isdigit(token[0]) || (token.size() > 1 && isdigit(token[1]))) {
            st.push(std::stod(token));
        }
        else if (token.size() == 1 && isOperator(token[0])) {
            double b = st.top(); st.pop();
            double a = st.top(); st.pop();
            char op = token[0];
            if (op == '+') st.push(a + b);
            else if (op == '-') st.push(a - b);
            else if (op == '*') st.push(a * b);
            else if (op == '/') st.push(a / b);
            else if (op == '^') st.push(pow(a, b));
        }
    }
    return st.top();
}

int main() {
    std::string expr = "(1 + 3)/10*5-((2+3)*2+3) + A";
    
    std::cout << "Инфикс: " << expr << std::endl;
    
    std::vector<std::string> postfix = toPostfix(expr);
    
    std::cout << "Постфикс: ";
    for (const auto& s : postfix) {
        std::cout << s << " ";
    }
    std::cout << std::endl;
    
    double result = calculate(postfix);
    if (!postfix.empty() && postfix[0] != "ERROR_LETTER") {
        std::cout << "Результат: " << result << std::endl;
    }    
    return 0;
}
