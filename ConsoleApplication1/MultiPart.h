#ifndef MultiPart_h__
#define MultiPart_h__
#include <vector>
#include <unordered_map>
class MultiPartParser
{
public:
	MultiPartParser() = default;
	~MultiPartParser() = default;
	/// Get parameters, This method should be called after calling the parse ()
	/// method.
	const std::unordered_map<std::string, std::string>& getParameters() const;

	/// Parse the http request stream to get files and parameters.
	int parse(const HttpRequestPtr& req);

protected:
	std::unordered_map<std::string, std::string> parameters_;
	int parse(const HttpRequestPtr& req,
		const char* boundaryData,
		size_t boundaryLen);
	int parseEntity(const char* begin, const char* end);
};
#endif // MultiPart_h__
