/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CheckConfig.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: menasy <menasy@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 12:42:07 by menasy            #+#    #+#             */
/*   Updated: 2025/05/04 18:46:40 by menasy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CheckConfig_HPP
# define CheckConfig_HPP
# include "Tokenizer.hpp"
# include "Tokenizer.hpp"
# include <iostream>
# include "HelperClass.hpp"
class CheckConfig : public Tokenizer
{
	private:
		bool inServer;
		std::string fileName;
		std::ifstream confFile;
	public:
		CheckConfig();
		CheckConfig(std::string argv);
		~CheckConfig();
		CheckConfig(const CheckConfig &other);
		CheckConfig &operator=(const CheckConfig &other);
	
		void checkConfig();
		void checkFileExtensions() ;
		std::string fileHandler();
		void bracketsCheck(std::string str);
		void checkElements(std::string str);
		void checkConfKey(const std::string& element);
		void emptyValueCheck();
		void checkValue();


};
#endif