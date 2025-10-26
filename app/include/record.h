#ifndef RECORD_H
#define RECORD_H

#include <bits/stdc++.h>

#define BLOCK_SIZE 4096
#define MAP_SIZE 1021441
#define MAX_REC_ID 162450
#define REC_SIZE 1496

time_t parseDateTime(const std::string& datetime_str) {
  struct tm tm = {};
  std::istringstream ss(datetime_str);
  ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
  return mktime(&tm);
}

std::string formatDateTime(time_t timestamp) {
  char buffer[32];
  struct tm* tm_info = localtime(&timestamp);
  strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm_info);
  return std::string(buffer);
}

struct Record {
  int id;
  char title[300];
  int year;
  char authors[150];
  int cites;
  time_t dateTime;
  char snippet[1024];

  Record() : id(0), year(0), cites(0), dateTime(0) {
    memset(snippet, 0, sizeof(snippet));
    memset(title, 0, sizeof(title));
    memset(authors, 0, sizeof(authors));
  }

  Record(std::vector<std::string>& fields) {
    id = std::stoi(fields[0]);
    year = std::stoi(fields[2]);
    cites = std::stoi(fields[4]);

    strncpy(title, fields[1].c_str(), sizeof(title));
    strncpy(authors, fields[3].c_str(), sizeof(authors));
    strncpy(snippet, fields[6].c_str(), sizeof(snippet));

    dateTime = parseDateTime(fields[5]);
  }

  void print() {
    std::cout << "         ID: " << id << std::endl;
    std::cout << "     Título: " << title << std::endl;
    std::cout << "        Ano: " << year << std::endl;
    std::cout << "    Autores: " << authors << std::endl;
    std::cout << "   Citações: " << cites << std::endl;
    std::cout << "Atualização: " << formatDateTime(dateTime) << std::endl;
    std::cout << "    Snippet: " << snippet << std::endl;
  }
};

#endif
