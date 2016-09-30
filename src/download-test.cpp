#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <curl/curl.h>
#include <regex>
struct CvmUrlData {
  std::stringstream headerstr;
  std::ofstream cvmfile;
};
void set_filename_from_header ( CvmUrlData & urldata ) {
  std::regex rgx ("Content-disposition: attachment; filename="
      "([0-9]+\\.zip)[^]*");
  std::smatch match_filename;
  std::string line;
  while (getline ( urldata.headerstr, line ) ) {
    const std::string line1 ( line );
    if ( std::regex_search ( line1.begin(), line1.end(), match_filename, 
          rgx ) )  break;
  }
  std::cout << "Got filename: " << match_filename[1] << "\n";
  std::string filename = "/tmp/";
  filename.append ( match_filename[1] );
  urldata.cvmfile.open ( filename );
  std::cout << "Opened filename: " << filename << "\n";
}

size_t header_cb(char *contents, size_t size, size_t nmemb, 
    CvmUrlData  * urldata)
{
  urldata->headerstr << contents;
  return size*nmemb;
}
size_t CurlWrite_CallbackFunc_StdString(char *contents, size_t size, size_t nmemb, CvmUrlData * urldata)
{
  if ( !urldata->cvmfile.is_open() ) 
    set_filename_from_header ( *urldata );
    try
    {
      urldata->cvmfile.write(contents, size*nmemb);
    }
  catch(std::bad_alloc &e)
  {
    std::cerr << "Error: ran out of memory\n.";
    return 0;
  }
  return size*nmemb;
}
int main()
{
  CURL *curl;
  CURLcode res;

  curl_global_init(CURL_GLOBAL_DEFAULT);

  curl = curl_easy_init();
  CvmUrlData urldata;
  if(curl)
  {
    curl_easy_setopt( curl, CURLOPT_URL, 
        "https://www.rad.cvm.gov.br/enetconsulta/frmDownloadDocumento.aspx?"
        "CodigoInstituicao=1&NumeroSequencialDocumento=59389" );
    //    curl_easy_setopt ( curl, CURLOPT_POSTFIELDS,
    //        "CodigoInstituicao=1&NumeroSequenciaDocumento=59389" );
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_cb);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &urldata);
    curl_easy_setopt( curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, 
        CurlWrite_CallbackFunc_StdString);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &urldata);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);


    res = curl_easy_perform(curl);
    if(res != CURLE_OK)
    {
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
          curl_easy_strerror(res));
    }

    curl_easy_cleanup(curl);
  }
}
