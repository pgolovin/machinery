Library:NewObject{
    name        = "Cube",
    elementName = "Cube",
}

Library:NewObject{
    name        = "Cilinder",
    elementName = "Cilinder",
}

Library:NewObject{
    name        = "Wedge",
    elementName = "Wedge",
}

Constructor:PlaceObject{name = "Cilinder", position = { x = 16,  y = 0, z = 16}}
Constructor:PlaceObject{name = "Cube",     position = { x = 16,  y = 1, z = 16}}
Constructor:PlaceObject{name = "Cilinder", position = { x = 16,  y = 2, z = 16}}

Constructor:PlaceObject{name = "Wedge", position = { x = 17, y = 0, z = 15 }, orientation = Directions.pX}
Constructor:PlaceObject{name = "Wedge", position = { x = 17, y = 0, z = 16 }, orientation = Directions.pX}
Constructor:PlaceObject{name = "Wedge", position = { x = 17, y = 0, z = 17 }, orientation = Directions.pX}

Constructor:PlaceObject{name = "Wedge", position = { x = 15, y = 0, z = 15 }, orientation = Directions.nX}
Constructor:PlaceObject{name = "Wedge", position = { x = 15, y = 0, z = 16 }, orientation = Directions.nX}
Constructor:PlaceObject{name = "Wedge", position = { x = 15, y = 0, z = 17 }, orientation = Directions.nX}

Constructor:PlaceObject{name = "Wedge", position = { x = 16, y = 0, z = 15 }, orientation = Directions.nZ}
Constructor:PlaceObject{name = "Wedge", position = { x = 16, y = 0, z = 17 }, orientation = Directions.pZ}

