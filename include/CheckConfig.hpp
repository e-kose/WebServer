/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CheckConfig.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekose <ekose@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 12:42:07 by menasy            #+#    #+#             */
/*   Updated: 2025/06/29 11:01:49 by ekose            ###   ########.fr       */
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
	
		std::string		fileHandler();
		
		void			checkConfig();
		void			checkFileExtensions() ;
		void 			bracketsCheck(std::string str);
		void 			checkElements(std::string str);
		void 			checkConfKey(const std::string& element);
		void 			checkRequiredElements();
};
#endif