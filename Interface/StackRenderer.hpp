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
    const int tileY = 230;

    //Card rendering adjusters
    bool spread;
    int offsetX = tileX;
	int offsetY = tileY / 6;
	int overlapX = 0;
	int overlapY = 5;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const {
    	states.texture = &cardset;
    	states.transform *= getTransform();
    	target.draw(vertices, states);
    }

    void resize() {
    	setSize(sf::Vector2i(tileX, tileY * stack->get_count()));
    	setOrigin(0, 0);
    	vertices.resize(4 * stack->get_count());
    }

public:
	StackRenderer(Solisp::Stack *stack) : Node(1) {
		this->stack = stack;

        setScale(1, 0.75);
        setPosition(stack->x * 75 + 50, stack->y * 50 + 30);
        UpdateList::addNode(this);

        spread = stack->get_tag(SPREAD);
        if(stack->get_tag(SPREAD_HORIZONTAL)) {
        	offsetX /= 6;
        	overlapX = 5;
        	offsetY = tileY;
        	overlapY = 0;
        } else if(!spread) {
        	offsetY = tileY;
        	overlapY = 0;
        }

        //Set up buffer texture
        buffer = new sf::RenderTexture();
        if(!buffer->create(tileX, 20 * tileY))
            throw std::logic_error("Error creating buffer");
        vertices.setPrimitiveType(sf::Quads);

        reload();
	}

	void reload() {
		int i = (offsetX == tileX) ? 0 : stack->get_count() - 1;
		int j = (offsetY == tileY) ? 0 : stack->get_count() - 1;
		Solisp::Card *card = stack->get_card();
		resize();

		if(card != NULL) {
			// get the current tile
	        int tileNumber = card->get_frame();
	        int tu = tileNumber % 13;
	        int tv = tileNumber / 13;

	        //Display on vertex
	        sf::Vertex* quad = &vertices[(j + i) * 4];

	        // define its 4 texture coordinates
	        quad[0].texCoords = sf::Vector2f(tu * tileX, tv * tileY);
	        quad[1].texCoords = sf::Vector2f((tu + 1) * tileX, tv * tileY);
	        quad[2].texCoords = sf::Vector2f((tu + 1) * tileX, (tv + 1) * tileY);
	        quad[3].texCoords = sf::Vector2f(tu * tileX, (tv + 1) * tileY);

	        // define its 4 corners
	        quad[0].position = sf::Vector2f(i * offsetX, j * offsetY);
	        quad[1].position = sf::Vector2f(i * offsetX + tileX, j * offsetY);
	        quad[2].position = sf::Vector2f(i * offsetX + tileX, j * offsetY + tileY);
	        quad[3].position = sf::Vector2f(i * offsetX, j * offsetY + tileY);

			while(card->get_next() != NULL && spread) {
				--j;
		    	card = card->get_next();

				// get the current tile
		        tileNumber = card->get_frame();
		        tu = tileNumber % 13;
		        tv = tileNumber / 13;

		        //Display on vertex
		        quad = &vertices[(j + i) * 4];

		        // define its 4 texture coordinates
		        quad[0].texCoords = sf::Vector2f(tu * tileX, tv * tileY);
		        quad[1].texCoords = sf::Vector2f(tu * tileX + offsetX + overlapX, tv * tileY);
		        quad[2].texCoords = sf::Vector2f(tu * tileX + offsetX + overlapX, tv * tileY + offsetY + overlapY);
		        quad[3].texCoords = sf::Vector2f(tu * tileX, tv * tileY + offsetY + overlapY);

		        // define its 4 corners
		        quad[0].position = sf::Vector2f(i * offsetX, j * offsetY);
		        quad[1].position = sf::Vector2f((i + 1) * offsetX + overlapX, j * offsetY);
		        quad[2].position = sf::Vector2f((i + 1) * offsetX + overlapX, (j + 1) * offsetY + overlapY);
		        quad[3].position = sf::Vector2f(i * offsetX, (j + 1) * offsetY + overlapY);
			}
	    }

        //Draw to buffer
        buffer->clear(sf::Color::Transparent);
        buffer->draw(vertices, sf::RenderStates(&cardset));
        buffer->display();
        setTexture(buffer->getTexture());
	}
};