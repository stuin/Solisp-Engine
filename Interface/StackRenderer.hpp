#include "Skyrmion/Node.h"
#include "enums.h"

#include <algorithm>

sf::Texture cardset;

class StackRenderer : public Node {
public:
	Solisp::Stack *stack;
    bool spread;

private:
	int index;

	//Graphical variables
    sf::VertexArray vertices;
    sf::RenderTexture *buffer;

    //Card image sizes
    const int tileX = 148;
    const int tileY = 230;

    //Card rendering adjusters
    int offsetX = tileX;
	int offsetY = tileY / 5;
	int overlapX = 0;
	int overlapY = 5;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const {
    	states.texture = &cardset;
    	states.transform *= getTransform();
    	target.draw(vertices, states);
    }

public:
	StackRenderer(Solisp::Stack *stack, int index) : Node(index == 0 ? POINTER : STACKS) {
		this->stack = stack;
		this->index = index;

        setScale(1, 0.75);
        setPosition(stack->x * 75 + 50, stack->y * 50 + 30);
        UpdateList::addNode(this);

        spread = stack->get_tag(SPREAD);
        if(stack->get_tag(SPREAD_HORIZONTAL)) {
        	offsetX /= 5;
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

	void reload(int count = -1, int skip = 0) {
		//Calculate proper count
		if(!spread)
			count = 1;
		else {
			if(count == -1)
				count = stack->get_count();
			count -= skip;
		}

		//Apply sizing
		int i = (offsetX == tileX) ? 0 : count - 1;
		int j = (offsetY == tileY) ? 0 : count - 1;
		setSize(sf::Vector2i(tileX * std::max(i + 1, 1), tileY * 0.75 * std::max(j + 1, 1)));
		setOrigin(0, 0);
    	vertices.resize(4 * std::max(count, 1));

		//Get first card
		Solisp::Card *card = stack->get_card();
		while(skip-- > 0 && card != NULL)
			card = card->get_next();

		if(card != NULL && count > 0) {
			// get the current tile
	        int tileNumber = card->get_frame();
	        if(card->is_hidden())
	        	tileNumber = 54;
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
				(overlapY > 0) ? --j : --i;
		    	card = card->get_next();

				// get the current tile
		        tileNumber = card->get_frame();
	        	if(card->is_hidden())
	        		tileNumber = 54;
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
	    } else {
	    	vertices[0].position = sf::Vector2f(0, 0);
            vertices[1].position = sf::Vector2f(0, 0);
            vertices[2].position = sf::Vector2f(0, 0);
            vertices[3].position = sf::Vector2f(0, 0);
	    }

        //Draw to buffer
        buffer->clear(sf::Color::Transparent);
        buffer->draw(vertices, sf::RenderStates(&cardset));
        buffer->display();
        setTexture(buffer->getTexture());
	}

	int getIndex() {
		return index;
	}

	sf::Vector2f getOffset(int count) {
		if(!spread)
			count = 0;
		return sf::Vector2f(
			(offsetX % tileX) * count,
			(offsetY % tileY) * count * 0.75);
	}

	int checkOffset(sf::Vector2f pos) {
		if(!spread)
			return 0;
		int count = std::max(stack->get_count() - 1, 0);
		if(overlapY > 0)
			return std::min((int)(pos.y / (offsetY * 0.75)), count);
		return std::min((int)(pos.x / (offsetX * 0.75)), count);
	}
};