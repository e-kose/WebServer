/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CheckConfig.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: menasy <menasy@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 12:42:07 by menasy            #+#    #+#             */
/*   Updated: 2025/05/06 15:42:08 by menasy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CheckConfig_HPP
# define CheckConfig_HPP
# include "Tokenizer.hpp"
# include "Tokenizer.hpp"
# include "WebServer.hpp"
# include <iostream>
# include "HelperClass.hpp"
class CheckConfig : public Tokenizer
{
	private:
		bool inServer;
		std::string fileName;
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