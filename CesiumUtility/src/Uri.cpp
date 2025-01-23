#include <CesiumUtility/Uri.h>

#include <ada/character_sets-inl.h>
#include <ada/implementation.h>
#include <ada/unicode.h>
#include <ada/url_aggregator.h>
#include <ada/url_search_params.h>

#include <cstdlib>
#include <cstring>
#include <functional>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

namespace CesiumUtility {

using UrlResult = ada::result<ada::url_aggregator>;

namespace {
const std::string HTTPS_PREFIX = "https:";
const std::string FILE_PREFIX = "file:///";
const char WINDOWS_PATH_SEP = '\\';
const char PATH_SEP = '/';

// C++ locale settings might change which values std::isalpha checks for. We
// only want ASCII.
bool isAsciiAlpha(unsigned char c) {
  return c >= 0x41 && c <= 0x7a && (c <= 0x5a || c >= 0x61);
}
bool isAscii(unsigned char c) { return c <= 0x7f; }

/**
 * A URI has a valid scheme if it starts with an ASCII alpha character and has a
 * sequence of ASCII characters followed by a "://"
 */
bool urlHasScheme(const std::string& uri) {
  for (size_t i = 0; i < uri.length(); i++) {
    unsigned char c = static_cast<unsigned char>(uri[i]);
    if (c == ':') {
      return uri.length() > i + 2 && uri[i + 1] == '/' && uri[i + 2] == '/';
    } else if ((i == 0 && !isAsciiAlpha(c)) || !isAscii(c)) {
      // Scheme must start with an ASCII alpha character and be an ASCII string
      return false;
    }
  }

  return false;
}
} // namespace

Uri::Uri(const std::string& uri) {
  UrlResult result;
  if (uri.starts_with("//")) {
    // This is a protocol-relative URL.
    // We will treat it as an HTTPS URL.
    this->hasScheme = true;
    result = ada::parse(HTTPS_PREFIX + uri);
  } else {
    this->hasScheme = urlHasScheme(uri);
    result = this->hasScheme ? ada::parse(uri) : ada::parse(FILE_PREFIX + uri);
  }

  if (result) {
    this->url =
        std::make_unique<ada::url_aggregator>(std::move(result.value()));
    this->params =
        std::make_unique<ada::url_search_params>(this->url->get_search());
  }
}

Uri::Uri(const Uri& uri) {
  if (uri.url) {
    this->url = std::make_unique<ada::url_aggregator>(*uri.url);
    this->params =
        std::make_unique<ada::url_search_params>(this->url->get_search());
  }
  this->hasScheme = uri.hasScheme;
}

Uri::Uri(const Uri& base, const std::string& relative, bool useBaseQuery) {
  UrlResult result;
  if (!base.isValid()) {
    this->hasScheme = urlHasScheme(relative);
    result = this->hasScheme ? ada::parse(relative)
                             : ada::parse(FILE_PREFIX + relative);
  } else {
    this->hasScheme = base.hasScheme;
    result = ada::parse(relative, base.url.get());
  }

  if (result) {
    this->url =
        std::make_unique<ada::url_aggregator>(std::move(result.value()));
    this->params =
        std::make_unique<ada::url_search_params>(this->url->get_search());

    if (useBaseQuery) {
      // Set from relative to base to give priority to relative URL query string
      for (const auto& [key, value] : *base.params) {
        if (!this->params->has(key)) {
          this->params->set(key, value);
        }
      }
      this->url->set_search(this->params->to_string());
    }
  }
}

std::string Uri::toString() const {
  if (!this->url) {
    return "";
  }

  // Update URL with any param modifications
  this->url->set_search(this->params->to_string());
  const std::string_view result = this->url->get_href();
  return this->hasScheme ? std::string(result)
                         : std::string(result.substr(FILE_PREFIX.length()));
}

bool Uri::isValid() const {
  return this->url != nullptr && this->params != nullptr;
}

const std::optional<std::string_view>
Uri::getQueryValue(const std::string& key) const {
  if (!this->isValid()) {
    return std::nullopt;
  }

  return this->params->get(key);
}

void Uri::setQueryValue(const std::string& key, const std::string& value) {
  if (!this->isValid()) {
    return;
  }

  this->params->set(key, value);
}

const std::string_view Uri::getPath() const {
  if (!this->isValid()) {
    return {};
  }

  // Remove leading '/'
  return this->url->get_pathname();
}

void Uri::setPath(const std::string_view& path) {
  this->url->set_pathname(path);
}

std::string Uri::resolve(
    const std::string& base,
    const std::string& relative,
    bool useBaseQuery,
    [[maybe_unused]] bool assumeHttpsDefault) {
  return Uri(Uri(base), relative, useBaseQuery).toString();
}

std::string Uri::addQuery(
    const std::string& uri,
    const std::string& key,
    const std::string& value) {
  Uri parsedUri(uri);
  if (!parsedUri.isValid()) {
    return uri;
  }

  parsedUri.setQueryValue(key, value);
  return parsedUri.toString();
}

std::string Uri::getQueryValue(const std::string& uri, const std::string& key) {
  return std::string(Uri(uri).getQueryValue(key).value_or(""));
}

std::string Uri::substituteTemplateParameters(
    const std::string& templateUri,
    const std::function<SubstitutionCallbackSignature>& substitutionCallback) {
  std::string result;
  result.reserve(templateUri.length());
  std::string placeholder;

  bool inPlaceholder = false;
  size_t startPos = 0;
  size_t currentPos = 0;

  // Iterate through the string, replacing placeholders where found
  while (currentPos < templateUri.length()) {
    const bool roomForEncodedChar = currentPos < templateUri.length() - 2;
    if(!inPlaceholder && (
      templateUri[currentPos] == '{' || 
      (roomForEncodedChar && templateUri[currentPos] == '%' && templateUri[currentPos + 1] == '7' && (templateUri[currentPos + 2] == 'B' || templateUri[currentPos + 2] == 'b')))) {
      inPlaceholder = true;
      startPos = currentPos + 1;
      // Skip past rest of encoded char if necessary
      if(templateUri[currentPos] == '%') {
        currentPos += 2;
      }
    } else if(inPlaceholder && (templateUri[currentPos] == '}' ||
    (roomForEncodedChar && templateUri[currentPos] == '%' && templateUri[currentPos + 1] == '7' && (templateUri[currentPos + 2] == 'D' || templateUri[currentPos + 2] == 'd')))) {
      placeholder = templateUri.substr(startPos, currentPos - startPos);
      result.append(substitutionCallback(placeholder));
      inPlaceholder = false;
      // Skip past rest of encoded char if necessary
      if(templateUri[currentPos] == '%') {
        currentPos += 2;
      }
    } else if(!inPlaceholder) {
      result += templateUri[currentPos];
    }

    ++currentPos;
  }

  if(inPlaceholder) {
    throw std::runtime_error("Unclosed template parameter");
  }

  // It's possible some placeholders were replaced with strings shorter than the placeholder itself, so we might need to shrink to fit
  result.shrink_to_fit();

  return result;
}

std::string Uri::escape(const std::string& s) {
  return ada::unicode::percent_encode(
      s,
      ada::character_sets::WWW_FORM_URLENCODED_PERCENT_ENCODE);
}

std::string Uri::unescape(const std::string& s) {
  return ada::unicode::percent_decode(s, s.find('%'));
}

std::string Uri::unixPathToUriPath(const std::string& unixPath) {
  return Uri::nativePathToUriPath(unixPath);
}

std::string Uri::windowsPathToUriPath(const std::string& windowsPath) {
  return Uri::nativePathToUriPath(windowsPath);
}

std::string Uri::nativePathToUriPath(const std::string& nativePath) {
  const std::string encoded = ada::unicode::percent_encode(
      nativePath,
      ada::character_sets::PATH_PERCENT_ENCODE);

  const bool startsWithDriveLetter =
      encoded.length() >= 2 &&
      isAsciiAlpha(static_cast<unsigned char>(encoded[0])) && encoded[1] == ':';

  std::string output;
  output.reserve(encoded.length() + (startsWithDriveLetter ? 1 : 0));

  // Paths like C:/... should be prefixed with a path separator
  if (startsWithDriveLetter) {
    output += PATH_SEP;
  }

  // All we really need to do from here is convert our slashes
  for (size_t i = 0; i < encoded.length(); i++) {
    if (encoded[i] == WINDOWS_PATH_SEP) {
      output += PATH_SEP;
    } else {
      output += encoded[i];
    }
  }

  return output;
}

std::string Uri::uriPathToUnixPath(const std::string& uriPath) {
  // URI paths are pretty much just unix paths with URL encoding
  const std::string_view& rawPath = uriPath;
  return ada::unicode::percent_decode(rawPath, rawPath.find('%'));
}

std::string Uri::uriPathToWindowsPath(const std::string& uriPath) {
  const std::string path =
      ada::unicode::percent_decode(uriPath, uriPath.find('%'));

  size_t i = 0;
  // A path including a drive name will start like /C:/....
  // In that case, we just skip the first slash and continue on
  if (path.length() >= 3 && path[0] == '/' &&
      isAsciiAlpha(static_cast<unsigned char>(path[1])) && path[2] == ':') {
    i++;
  }

  std::string output;
  output.reserve(path.length() - i);
  for (; i < path.length(); i++) {
    if (path[i] == PATH_SEP) {
      output += WINDOWS_PATH_SEP;
    } else {
      output += path[i];
    }
  }

  return output;
}

std::string Uri::uriPathToNativePath(const std::string& uriPath) {
#ifdef _WIN32
  return uriPathToWindowsPath(uriPath);
#else
  return uriPathToUnixPath(uriPath);
#endif
}

std::string Uri::getPath(const std::string& uri) {
  return std::string(Uri(uri).getPath());
}

std::string Uri::setPath(const std::string& uri, const std::string& newPath) {
  Uri parsedUri(uri);
  parsedUri.setPath(newPath);
  return parsedUri.toString();
}

} // namespace CesiumUtility
