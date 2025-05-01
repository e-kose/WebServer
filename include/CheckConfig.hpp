/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CheckConfig.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekose <ekose@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 12:42:07 by menasy            #+#    #+#             */
/*   Updated: 2025/05/01 18:04:24 by ekose            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CheckConfig_HPP
# define CheckConfig_HPP
# include "Tokenizer.hpp"
# include "Parser.hpp"
# include <iostream>
class CheckConfig : public Parser
{
	private:
		bool inServer;
	public:
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