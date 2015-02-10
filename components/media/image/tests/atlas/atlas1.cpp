#include "shared.h"

using namespace media;
using namespace math;

void print (const Tile& tile)
{
  printf ("Tile name is '%s'\n", tile.name);
  printf ("Tile image is '%s'\n", tile.image);
  printf ("Tile origin is %ux%u\n", tile.origin.x, tile.origin.y);
  printf ("Tile size is %ux%u\n", tile.size.x, tile.size.y);
  printf ("Tile tag is %lu\n", tile.tag);
}

int main ()
{
  try
  {
    printf ("Results of atlas1 test:\n");

    Atlas atlas;

    printf ("Tiles count is %u\n", atlas.TilesCount ());

    Tile new_tile;

    new_tile.name   = "tile1";
    new_tile.image  = "atlas_image.tga";
    new_tile.origin = vec2ui (16, 8);
    new_tile.size   = vec2ui (8, 8);
    new_tile.tag    = 1;

    atlas.Insert (new_tile);
    atlas.Insert ("tile2", "atlas_image2.tga", vec2ui (8, 8), vec2ui (16, 16));

    printf ("Tiles count is %u\n", atlas.TilesCount ());

    printf ("Tile 1 is:\n");
    print (atlas.Tile (1));

    printf ("Tile 'tile1' is:\n");
    print (atlas.Tile ("tile1"));

    const Tile *founded_tile = atlas.Find ("tile2");

    if (founded_tile)
      print (*founded_tile);

    new_tile.name   = "tilex";
    new_tile.origin = vec2ui (24, 16);
    new_tile.size   = vec2ui (4, 4);
    new_tile.tag    = 2;

    atlas.Set (0, new_tile);
    atlas.Set (1, "tilexx", "atlas_imagexx.tga", vec2ui (18, 18), vec2ui (6, 6));

    printf ("Tiles count is %u\n", atlas.TilesCount ());

    printf ("Tile 0 is:\n");
    print (atlas.Tile ((unsigned int)0));
    printf ("Tile 1 is:\n");
    print (atlas.Tile (1));

    atlas.Remove ("tilexx");

    printf ("Tiles count is %u\n", atlas.TilesCount ());

    atlas.Remove ((unsigned int)0);

    printf ("Tiles count is %u\n", atlas.TilesCount ());

    atlas.Insert (new_tile);

    printf ("Tiles count is %u\n", atlas.TilesCount ());

    atlas.RemoveAllTiles ();

    printf ("Tiles count is %u\n", atlas.TilesCount ());
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n",exception.what ());
  }

  return 0;
}
