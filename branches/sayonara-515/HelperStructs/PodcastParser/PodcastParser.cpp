/*
 *
 * valid_document = first tag LIKE <rss version="2.0">
 * valid_item = channel.item.enclosore::(type).contains(audio) > channel.item.enclosure == NULL
 *
 *
 * MetaData.title = channel.item.title
 * MetaData.artist = itunes:author > channel.item.author
 * MetaData.album = channel.title
 * MetaData.year = channel.item.pubDate.year > channel.pubDate.yeicon.url = > item.image.url > itunes:image (1400x1400) px 
 *
 *
 *
 * */
