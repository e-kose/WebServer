#include <iostream>
#include "include/ServerConf.hpp"
#include "include/Tokenizer.hpp"

int main(int ac, char **av)
{
	if (ac != 2)
	{
		std::cerr << "Usage: " << av[0] << " <config_file>" << std::endl;
		return 1;
	}
	Tokenizer tok("server {\nlisten 8080 127.0.0.1;\nserver_name example.com www.example.com;\nroot /var/www/html;\nindex index.html;\nclient_max_body_size 5M;\nerror_page 404 /errors/404.html\n\nlocation / {\n  methods GET POST;\n  autoindex off;\n  index index.html;\n\n\nlocation /upload {\n  methods POST;\n  upload_store /var/www/uploads;\n}\n}\nserver {\nlisten       0.0.0.0:80;\nserver_name  example.com www.example.com;\nroot         /var/www/html;\nindex        index.html index.htm index.php;\naccess_log   /var/log/myserver/access.log  main;\nerror_log    /var/log/myserver/error.log   error;\nsendfile     on;\ntcp_nopush   on;\nkeepalive_timeout 65;\nclient_max_body_size 10M;\ninclude      mime.types;\ndefault_type application/octet-stream;\n# Hata sayfaları\nerror_page 404 /errors/404.html;\nerror_page 500 502 503 504 /errors/50x.html;\nlocation = /errors/50x.html {\n    root /var/www/html;\n}\n# Static kök dizin\nlocation / {\n    try_files $uri $uri/ =404;\n}\n}");
	tok.seperation();
	std::vector<std::string> s = tok.getText();
	std::vector<std::string>::iterator it = s.begin();
	for (;it != s.end(); it++){
		std::cout << *it << ' ';
	}
	return 0;


}