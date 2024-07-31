#include <iostream>
#include <regex>
#include <string>

#include "curl/curl.h"

using namespace std;

// callback function for libcurl
size_t writeCallback(char *buf, size_t size, size_t nmemb, void *up) {
  for (int c = 0; c < size * nmemb; c++) {
    ((string *)up)->push_back(buf[c]);
  }
  return size * nmemb;
}

string fetchHtml(string url) {
  CURL *curl;
  CURLcode res;
  string html;

  curl_global_init(CURL_GLOBAL_ALL);
  curl = curl_easy_init();
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, 102400L);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &html);

    // libcurl to follow redirection
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    // some websites need to set user agent to get the correct html
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl/8.5.0");
    curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_2TLS);
    curl_easy_setopt(curl, CURLOPT_FTP_SKIP_PASV_IP, 1L);
    curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);

    // debug output
    // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
  }
  return html;
}

// remove useless characters
string rm_useless_char(string str) {
  for (int i = 0, len = str.size(); i < len; i++) {
    // check whether character is punctuation or digit
    if (ispunct(str[i]) || isdigit(str[i])) {
      str.erase(i--, 1);
      len = str.size();
    }
  }
  return str;
}

// regex match and return the match
string regex_match(string reg, string mem, int idx) {
  regex treg(reg);
  smatch result;
  regex_search(mem, result, treg);

  return result[idx];
}

void info_twitter(string url, string base_url) {
  string html = fetchHtml(url);

  // remove trailing / from base_url
  base_url.pop_back();

  // regex_match the html with the following regexs
  cout << "Full Name: "
       << regex_match(
              "<a class=\"profile-card-fullname\" href=\"[^\"]+\" "
              "title=\"([^\"]+)\">",
              html, 1)
       << endl;

  cout << "Username: "
       << regex_match(
              "<a class=\"profile-card-username\" href=\"[^\"]+\" "
              "title=\"([^\"]+)\">",
              html, 1)
       << endl;

  cout << "Banner: " << base_url
       << regex_match(
              "<div class=\"profile-banner\"><a href=\"([^<]+)\" "
              "target=\"_blank\">",
              html, 1)
       << endl;

  cout << "Avatar: " << base_url
       << regex_match(
              "<a class=\"profile-card-avatar\" href=\"([^<]+)\" "
              "target=\"_blank\">",
              html, 1)
       << endl;

  cout << "Bio: " << regex_match("<div class=\"profile-bio\"><p dir=\"auto\">([^<]+)</p></div>", html, 1) << endl;

  cout << "Location: "
       << regex_match(
              "<div class=\"icon-container\"><span "
              "class=\"icon-location\" "
              "title=\"\"></span></div>\\s*</span>\\s*<span>([^<]+)</span>",
              html, 1)
       << endl;

  cout << "Joined On: "
       << regex_match(
              "<div class=\"profile-joindate\"><span "
              "title=\"([^\"]+)\">",
              html, 1)
       << endl;

  cout << "Tweets: "
       << regex_match(
              "<li class=\"posts\">\\s*<span "
              "class=\"profile-stat-header\">Tweets</span>\\s*<span "
              "class=\"profile-stat-num\">([0-9,]+)</span>",
              html, 1)
       << endl;

  cout << "Following: "
       << regex_match(
              "<li class=\"following\">\\s*<span "
              "class=\"profile-stat-header\">Following</span>\\s*<span "
              "class=\"profile-stat-num\">([0-9,]+)</span>",
              html, 1)
       << endl;

  cout << "Followers: "
       << regex_match(
              "<li class=\"followers\">\\s*<span "
              "class=\"profile-stat-header\">Followers</span>\\s*<span "
              "class=\"profile-stat-num\">([0-9,]+)</span>",
              html, 1)
       << endl;

  cout << "Likes: "
       << regex_match(
              "<li class=\"likes\">\\s*<span "
              "class=\"profile-stat-header\">Likes</span>\\s*<span "
              "class=\"profile-stat-num\">([0-9,]+)</span>",
              html, 1)
       << endl;
}

void info_instagram(string url) {
  string html = fetchHtml(url);

  // regex_match the html with the following regexs
  cout << "Real Name: " << rm_useless_char(regex_match("See Instagram photos and videos from ([^\\(]+) \\(", html, 1))
       << endl;
  cout << "Following: " << regex_match("(([0-9,]+)(K|M|B)?) Following", html, 1) << endl;
  cout << "Posts: " << regex_match("(([0-9,]+)(K|M|B)?) Posts", html, 1) << endl;
  cout << "Followers: " << regex_match("(([0-9,]+)(K|M|B)?) Followers", html, 1) << endl;
}

void info_github(string url) {
  string html = fetchHtml(url);

  // regex_match the json with the following regexs
  cout << "Login: " << regex_match("\"login\": \"([^\"]+)\"", html, 1) << endl;
  cout << "ID: " << regex_match("\"id\": ([0-9]+)", html, 1) << endl;
  cout << "Avatar: " << regex_match("\"avatar_url\": \"([^\"]+)\"", html, 1) << endl;
  cout << "Type: " << regex_match("\"type\": \"([^\"]+)\"", html, 1) << endl;
  cout << "Name: " << regex_match("\"name\": \"([^\"]+)\"", html, 1) << endl;
  cout << "Company: " << regex_match("\"company\": ([^\"]+),", html, 1) << endl;
  cout << "Blog: " << regex_match("\"blog\": \"([^\"]+)\",", html, 1) << endl;
  cout << "Location: " << regex_match("\"location\": ([^\"]+),", html, 1) << endl;
  cout << "Email: " << regex_match("\"email\": ([^\"]+),", html, 1) << endl;
  cout << "Hireable: " << regex_match("\"hireable\": ([^\"]+),", html, 1) << endl;
  cout << "Bio: " << regex_match("\"bio\": \"([^\"]+)\"", html, 1) << endl;
  cout << "Twitter Username: " << regex_match("\"twitter_username\": \"([^\"]+)\"", html, 1) << endl;
  cout << "Public Repos: " << regex_match("\"public_repos\": ([^\"]+),", html, 1) << endl;
  cout << "Public Gists: " << regex_match("\"public_gists\": ([^\"]+),", html, 1) << endl;
  cout << "Followers: " << regex_match("\"followers\": ([^\"]+),", html, 1) << endl;
  cout << "Following: " << regex_match("\"following\": ([^\"]+),", html, 1) << endl;
  cout << "Created At: " << regex_match("\"created_at\": \"([^\"]+)\"", html, 1) << endl;
  cout << "Updated At: " << regex_match("\"updated_at\": \"([^\"]+)\"", html, 1) << endl;
}

// function to ask user to input username with parmeters of url and name
string ask_user(string url, string name) {
  string user;
  cout << "Enter username: ";
  cin >> user;
  url += user;
  cout << endl << "Searching " << name << "..." << endl << endl;
  return url;
}

int main(void) {
  // using [nitter](https://github.com/zedeus/nitter) because twitter doesn't
  // work anymore
  const string twitter_url = "https://nitter.privacydev.net/";

  const string instagram_url = "https://www.instagram.com/";
  const string github_url = "https://api.github.com/users/";
  string choice;
  char site;

  while (true) {
    cout << endl << "Enter website ([t]witter [i]nstagram [g]ithub e[x]it) : ";
    cin >> choice;
    site = tolower(choice[0]);

    switch (site) {
      case 't':
        info_twitter(ask_user(twitter_url, "Twitter"), twitter_url);
        break;
      case 'i':
        info_instagram(ask_user(instagram_url, "Instagram"));
        break;
      case 'g':
        info_github(ask_user(github_url, "Github"));
        break;
      case 'x':
        return 0;
      default:
        cout << "Invalid website" << endl;
        break;
    }
  }
}
