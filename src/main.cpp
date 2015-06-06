/* (c) Copyright 2011-2014 Felipe Magno de Almeida
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>

#include <gntl/parser/libxml2/dom/document.hpp>
#include <gntl/parser/libxml2/dom/xml_document.hpp>

#include <boost/program_options.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

template <typename Media>
void print_media_sources_media(Media const& media)
{
  typedef gntl::concept::parser::media_traits<Media> media_traits;
  std::cout << media_traits::source(media) << std::endl;
}

template <typename Switch>
void print_media_sources_switch(Switch const& switch_)
{
  typedef gntl::concept::parser::switch_traits<Switch> switch_traits;

  typedef typename switch_traits::context_range context_range;
  context_range contexts = switch_traits::context_all(switch_);
  typedef typename boost::range_iterator<context_range>::type context_iterator;
  for(context_iterator first = boost::begin(contexts), last = boost::end(contexts)
        ;first != last;++first)
  {
    print_media_sources_context(*first);
  }

  // Not yet implemented in gntl
  // typedef typename switch_traits::switch_range switch_range;
  // switch_range switches = switch_traits::switch_all(switch_);
  // typedef typename boost::range_iterator<switch_range>::type switch_iterator;
  // for(switch_iterator first = boost::begin(switches), last = boost::end(switches)
  //       ;first != last;++first)
  // {
  //   print_media_sources_switch(*first);
  // }

  typedef typename switch_traits::media_range media_range;
  media_range medias = switch_traits::media_all(switch_);
  typedef typename boost::range_iterator<media_range>::type media_iterator;
  for(media_iterator first = boost::begin(medias), last = boost::end(medias)
        ;first != last;++first)
  {
    print_media_sources_media(*first);
  }
}                        

template <typename Context>
void print_media_sources_context(Context const& context)
{
  typedef gntl::concept::parser::context_traits<Context> context_traits;

  typedef typename context_traits::context_range context_range;
  context_range contexts = context_traits::context_all(context);
  typedef typename boost::range_iterator<context_range>::type context_iterator;
  for(context_iterator first = boost::begin(contexts), last = boost::end(contexts)
        ;first != last;++first)
  {
    print_media_sources_context(*first);
  }

  typedef typename context_traits::switch_range switch_range;
  switch_range switches = context_traits::switch_all(context);
  typedef typename boost::range_iterator<switch_range>::type switch_iterator;
  for(switch_iterator first = boost::begin(switches), last = boost::end(switches)
        ;first != last;++first)
  {
    print_media_sources_switch(*first);
  }

  typedef typename context_traits::media_range media_range;
  media_range medias = context_traits::media_all(context);
  typedef typename boost::range_iterator<media_range>::type media_iterator;
  for(media_iterator first = boost::begin(medias), last = boost::end(medias)
        ;first != last;++first)
  {
    print_media_sources_media(*first);
  }
}                        

void print_media_sources(gntl::parser::libxml2::dom::document const& document)
{
  typedef gntl::concept::parser::document_traits
    <gntl::parser::libxml2::dom::document> document_traits;

  gntl::parser::libxml2::dom::head head
    = document_traits::head(document);

  typedef document_traits::body_type body_type;
  body_type body = document_traits::body(document);

  typedef gntl::concept::parser::body_traits<body_type> body_traits;

  print_media_sources_context(body_traits::context(body));
}

int main(int argc, char* argv[])
{
  boost::filesystem::path ncl_file;
  {
    boost::program_options::options_description description
      ("Allowed options");
    description.add_options()
      ("help", "produce this help message")
      ("ncl", boost::program_options::value<std::string>(), "NCL file to play");

    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, description), vm);
    boost::program_options::notify(vm);

    if(vm.count("help"))
    {
      std::cout << description << std::endl;
      return 1;
    }

    if(vm.count("ncl"))
      ncl_file = vm["ncl"].as<std::string>();
    else
    {
      std::cout << description << std::endl;
      return 1;
    }
  }
    
  if(boost::filesystem::exists(ncl_file))
  {
    ::xmlParserCtxtPtr parser_context = ::xmlNewParserCtxt();
    ::xmlDocPtr xmldoc = ::xmlCtxtReadFile(parser_context, ncl_file.string().c_str(), 0, 0);
    if(!xmldoc)
    {
      std::cout << "File " << ncl_file.string() << " could not be XML parsed" << std::endl;
      return -1;
    }
    
    gntl::parser::libxml2::dom::xml_document xml_document(xmldoc);
    gntl::parser::libxml2::dom::document root_document(xml_document.root());
    print_media_sources(root_document);
  }
  else
  {
    std::cout << "File " << ncl_file.string() << " could not be found" << std::endl;
    return -1;
  }  
}
