
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daong <daong@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/09 00:49:24 by daong             #+#    #+#             */
/*   Updated: 2025/02/09 02:21:38 by daong            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <cstdlib>
#include "Array.hpp"

#define MAX_VAL 750
int main(int, char**)
{
    {
        std::cout << "\n--- testing *a ---" << std::endl;

        int * a = new int();
        std::cout << *a << std::endl;

        int * b = new int;
        std::cout << *b << std::endl;

        delete a;
        delete b;
    }
    {
        std::cout << "\n--- my own tests ---" << std::endl;
        Array<std::string> a1;
        std::cout << "a1 size before copy: " << a1.size() << std::endl;

        Array<std::string> a2(3);
        a2[0] = "hello";
        a2[1] = "world";
        a2[2] = "123";
        
        a1 = a2;
        std::cout << "a1 size after copy: " << a1.size() << std::endl;
        a1[2] = "456";

        for (unsigned int i = 0; i < a1.size(); i++) {
            std::cout << "a1[" << i << "]: " << a1[i] << "\ta2[" << i << "]: " << a2[i] << std::endl;
        }

    }
    {
        std::cout << "\n--- subject tests ---" << std::endl;   
        Array<int> numbers(MAX_VAL);
        int* mirror = new int[MAX_VAL];
            srand(time(NULL));
            for (int i = 0; i < MAX_VAL; i++)
            {
                const int value = rand();
                numbers[i] = value;
                mirror[i] = value;
            }
        //SCOPE
        {
            Array<int> tmp = numbers;
            Array<int> test(tmp);
        }

        for (int i = 0; i < MAX_VAL; i++)
        {
            if (mirror[i] != numbers[i])
            {
                std::cerr << "didn't save the same value!!" << std::endl;
                return 1;
            }
        }
        try
        {
            numbers[-2] = 0;
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        try
        {
            numbers[MAX_VAL] = 0;
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }

        for (int i = 0; i < MAX_VAL; i++)
        {
            numbers[i] = rand();
        }
        delete [] mirror;//
        return 0;
    }
}