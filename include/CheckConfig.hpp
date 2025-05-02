/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CheckConfig.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekose <ekose@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 12:42:07 by menasy            #+#    #+#             */
/*   Updated: 2025/05/03 00:24:33 by ekose            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CheckConfig_HPP
# define CheckConfig_HPP
# include "Tokenizer.hpp"
# include "Tokenizer.hpp"
# include <iostream>
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
};
#endif