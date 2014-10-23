// Modified from CSE422 FS09
// Modified from CSE422 FS12
// Modified from CSE422 FS13

// HTTP_Message - Base class for HTTP requests and responses.  Defines the
// methods for accessing the various headers on a request/response.  Also
// defines some internal things that are shared by the request/response classes.


#ifndef _HTTP_MESSAGE_H_
#define _HTTP_MESSAGE_H_

#include <map>
#include <string>
#include <vector>

namespace
{
    // The exact string of characters used to represent HTTP line endings.
    const std::string line_ending = "\r\n";

    // The character used to separate the name of a header from its value.
    const char header_delimiter = ':';
}


// Structure used to associate header names and values, whenever necessary.
struct HTTP_Header_Field
{
    std::string name;
    std::string value;
};


class HTTP_Message
{
public:
    virtual ~HTTP_Message();

    /*
     * Purpose: indicates how many headers the message has.
     * Receive: none
     * Return:  the number of headers stored in the message.
     */
    unsigned get_num_header_fields() const;

    /*
     * Purpose: copies all of the message's headers into the given vector.  
     *          Use this if you need to iterate through the headers.  If you 
     *          know the name of the header you want, get_header_value() is 
     *          far more useful.
     * Receive: out_set - will be set to a collection of HTTP_Header_Fields
     *                    representing all of the headers stored in the 
     *                    message
     * Return:  none
     */
    void get_header_set(std::vector<HTTP_Header_Field>& out_set) const;

    /*
     * Purpose: retrieves the value of the header with the given name.
     * Receive: name - the name of the header to look up.
     *          out_value - Will be set to that header's value, if it is 
     *                      found. If no header with that name is found, 
     *                      value will be undefined.
     * Return:  true if the requested header was found in the message
     *          (in which case, out_value is valid); false if it was 
     *          not found.
     */
    bool get_header_value(const std::string& name,
        std::string& out_value) const;


    /*
     * Purpose: Updates the message to have the given header field.  
     *          Overwrites the old value of the specified header if 
     *          the message already had it.
     * Receive: field - The name/value of the header to set.
     * Return:  none
     */
    void set_header_field(const HTTP_Header_Field& field);

    /*
     * Purpose: Updates the given header field in the message. If the 
     *          header is not already present, it will be added to the 
     *          message.  If the header *is* already present, its previous 
     *          value will be overwritten.
     * Receive: name - The name of the header to set.
     *          value - The new value to set.
     * Return:  none
     */
    void set_header_field(const std::string& name,
        const std::string& value);


protected:
    HTTP_Message();

    /*
     * Purpose: parse the received data to construct the object
     * Receive: the data to be parsed and the length of the data
     * Return:  true if the data is valid, false otherwise.
     */
    bool parse_fields(const char* data, unsigned length);

    /*
     * Purpose: construct a string that represents this message, for
     *          sending or other purposes.
     * Receive: output_string - the string to hold the message.
     * Return:  none
     */
    virtual void print(std::string& output_string) const;

    /*
     * Purpose: construct a string that represents this message, for
     *          sending or other purposes
     * Receive: output_buffer - the char array to hold the message.
     *          buffer_length - the lengthe of the buffer
     * Return:  none
     */
    virtual void print(char* output_buffer, unsigned buffer_length) const;

    /*
     * Purpose: copy the data_string into the buffer, if the buffer stil
     *          has room for this data_string.
     * Receive: output_buffer - the char array to store data_string
     *          data_string - the string to be copied
     *          remaining_length - the remaining room of the buffer
     * Return:  none
     */
    virtual void copy_if_room(char*& output_buffer, const char* data_string,
        unsigned& remaining_length) const;

    /*
     * Purpose: scan the data char-by-char until a newline char is found.
     * Receive: data - the char array to be scaned
     *          length - the length of the data
     * Return:  the pointer points to the beginning of next line.
     */
    const char* find_next_line(const char* data, unsigned length) const;


private:
    std::map<std::string, std::string> headers;
};

#endif //ndef _HTTP_MESSAGE_H_
