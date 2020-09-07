#include "Skyrmion/Node.h"

sf::Texture cardset;

class StackRenderer : public Node {
private:
	Solisp::Stack *stack;

	//Graphical variables
    sf::VertexArray vertices;
    sf::RenderTexture *buffer;

    //Card image sizes
    const int tileX = 148;
    const int tileY = 287;

    void resize() {
    	setSize(sf::Vector2i(tileX, tileY * stack->get_count()));
    	setOrigin(0, 0);
    	vertices.resize(4);
    }

public:
	StackRenderer(Solisp::Stack *stack) : Node(0) {
		this->stack = stack;
        vertices.setPrimitiveType(sf::Quads);

        //Set up buffer texture
        buffer = new sf::RenderTexture();
        if(!buffer->create(tileX, 20 * tileY))
            throw std::logic_error("Error creating buffer");

        reload();
	}

	void reload() {
		int i = 0;
		int j = 0;
		resize();

		if(stack->get_card() != NULL) {

			// get the current tile
	        int tileNumber = stack->get_card()->get_frame();
	        int tu = tileNumber % 13;
	        int tv = tileNumber / 13;

	        std::cout << tu * tileX << ", " << tv * tileY << "\n";

	        //Display on vertex
	        sf::Vertex* quad = &vertices[i * 4];

	        // define its 4 texture coordinates
	        quad[0].texCoords = sf::Vector2f(tu * tileX, tv * tileY);
	        quad[1].texCoords = sf::Vector2f((tu + 1) * tileX, tv * tileY);
	        quad[2].texCoords = sf::Vector2f((tu + 1) * tileX, (tv + 1) * tileY);
	        quad[3].texCoords = sf::Vector2f(tu * tileX, (tv + 1) * tileY);

	        // define its 4 corners
	        quad[0].position = sf::Vector2f(i * tileX, j * tileY);
	        quad[1].position = sf::Vector2f((i + 1) * tileX, j * tileY);
	        quad[2].position = sf::Vector2f((i + 1) * tileX, (j + 1) * tileY);
	        quad[3].position = sf::Vector2f(i * tileX, (j + 1) * tileY);
	    }

        //Draw to buffer
        buffer->clear(sf::Color::Transparent);
        buffer->draw(vertices, sf::RenderStates(&cardset));
        buffer->display();
        setTexture(buffer->getTexture());
	}
};