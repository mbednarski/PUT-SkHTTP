#ifndef _BODY_STREAM_H_
#define _BODY_STREAM_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>

// uzywa RAII i jest bezpiecznym sposobem na przechowywanie otrzymywanych/wysylanych danyc
// bezpicznym tj. zapewnia brak wyciekow pamieci
class ContentStream
{
private:
    // bufir na dane
	unsigned char *buffer;
    // rozmiar chunka - pojemnosc bufora jest jego wielokrotnoscia
	const size_t CHUNK_SIZE = 8*1024*1024;
    const size_t FILE_BUFFER_SIZE = 8*1024*1024;
    // aktualny rozmiar
	size_t currentSize;
    // aktualna pojemnosc
	size_t capacity;
    // powieksza bufor o chunk    
	void Resize(void);
public:
    // zapewnienie kopiowania, przenoszenia i przypisywania
	ContentStream(const ContentStream &obj);
	ContentStream &operator=(ContentStream obj);
    ContentStream(ContentStream &&obj);
	ContentStream(void);
    // dopisywanie danych w roznych formatach
	void Append(const void *data, size_t len);
	void Append(unsigned char c);
	void Append(const std::string &str);	
	void Append(const char *cstr);
    void Append(const ContentStream &stream);
    // pobranie wskaznika na wlasciwe dane i zwrocenie ich rozmiary przez parametr wyjsciowy
	const unsigned char *GetData(size_t &size) const;	
    
    // zapis zawartosci do pliku
    bool ToFile(const std::string &filename) const;
    
    // pobranie zawartosci jako string
	std::string AsString(void) const;
    
	~ContentStream();
    
    // dopisanie zawartosci plku do bufora
    void AppendFile(const std::string &filename);
    
};
#endif
	