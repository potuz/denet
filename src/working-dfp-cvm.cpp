#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <curl/curl.h>
#include <regex>

size_t CurlWrite_CallbackFunc_StdString(void *contents, size_t size, size_t nmemb, std::string *s)
{
  size_t newLength = size*nmemb;
  size_t oldLength = s->size();
  try
  {
    s->resize(oldLength + newLength);
  }
  catch(std::bad_alloc &e)
  {
    std::cerr << "Error: ran out of memory\n.";
    return 0;
  }

  std::copy((char*)contents,(char*)contents+newLength,s->begin()+oldLength);
  return size*nmemb;
}
int main()
{
  CURL *curl;
  CURLcode res;

  curl_global_init(CURL_GLOBAL_ALL);

  curl = curl_easy_init();
  std::string s;
  if(curl)
  {

    curl_easy_setopt( curl, CURLOPT_URL, 
        "http://siteempresas.bovespa.com.br/consbov/"
        "ExibeTodosDocumentosCVM.asp?CCVM=5380&CNPJ=82.643.537/0001-34" 
        "&TipoDoc=C&QtLinks=10");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, 
        "hdnCategoria=IDI2&hdnPagina=&FechaI=&FechaV=" );

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWrite_CallbackFunc_StdString);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);


    res = curl_easy_perform(curl);
    if(res != CURLE_OK)
    {
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
          curl_easy_strerror(res));
    }

    curl_easy_cleanup(curl);
  }
  std::istringstream mystream (s);
  std::string line;
  std::regex rgx (".*DFP - ENET - Ativo[^]*", std::regex::ECMAScript);
  std::regex rgx2 (
      ".*fVisualizaArquivo_ENET.\'([0-9]+)\',\'DOWNLOAD.*");
  std::regex rgxdate ( 
      ".*([0-9][0-9]/[0-9][0-9]/[0-9]{4})[^]*");
  std::regex rgxversion ( ".*nowrap>([0-9]\\.[0-9]).[^]*");
  std::smatch match_file, match_date, match_version;
  while ( getline( mystream, line ) )
  { 
    if ( std::regex_match ( line.begin(), line.end(), rgx ) ) {
      while (getline (mystream, line ) ) {
        const std::string line1 (line);
        if ( std::regex_search (line1.begin(), line1.end(), 
              match_file, rgx2 ) ) {
          while (getline (mystream, line ) ) {
            const std::string line2 (line);
            if (std::regex_search (line2.begin(), line2.end(),
                  match_date, rgxdate ) ) { 
              while ( getline (mystream, line ) ) {
                const std::string line3 (line);
                if (std::regex_search ( line3.begin(), line3.end(),
                      match_version, rgxversion ) ){
                  std::cout << "Found Active. File:" << match_file[1];
                  std::cout << "  Date: " << match_date[1];
                  std::cout << "  Version: "<< match_version[1] << "\n";
                  break;
                }
              }
              break;
            }
          }
          break;
        }
      }
    }
  }
}
