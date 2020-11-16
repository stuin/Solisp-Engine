#include "main.h"

#include <algorithm>

sf::Texture cardset;

class StackRenderer : public Node {
public:
	Solisp::Stack *stack;
	bool spread;

private:
	unc index;

	//Graphical variables
	sf::VertexArray vertices;
	sf::RenderTexture *buffer;

	//Card image sizes
	const int tileX = 103;
	const int tileY = 142;
	const int gapX = 80;
	const int gapY = 57;
	const float scaleX = 1.5;
	const float scaleY = 1.5;

	//Card rendering adjusters
	int offsetX = tileX;
	int offsetY = tileY / 6.5;
	int overlapX = 0;
	int overlapY = 5;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const {
		states.texture = &cardset;
		states.transform *= getTransform();
		target.draw(vertices, states);
	}

public:
	StackRenderer(Solisp::Stack *stack, unc index) : Node(index == 0 ? POINTER : STACKS) {
		this->stack = stack;
		this->index = index;

		setScale(scaleX, scaleY);
		setPosition(stack->x * gapX + 50, stack->y * gapY + 30);
		UpdateList::addNode(this);

		spread = stack->get_tag(SPREAD);
		if(stack->get_tag(SPREAD_HORIZONTAL)) {
			offsetX /= 4.5;
			overlapX = 5;
			offsetY = tileY;
			overlapY = 0;

			if(stack->get_tag(SPREAD_REVERSE)) {
				offsetX *= -1;
				overlapX *= -1;
				setPosition((stack->x + 3) * gapX + 50, stack->y * gapY + 30);
			}
		} else if(!spread) {
			offsetY = tileY;
			overlapY = 0;
		}

		//Set up buffer texture
		buffer = new sf::RenderTexture();
		if(!buffer->create(std::abs(tileX + 20 * offsetX), std::abs(tileY + 20 * offsetY)))
			throw std::logic_error("Error creating buffer");
		vertices.setPrimitiveType(sf::Quads);

		reload();
	}

	void reload(int count = 0, int skip = 0) {
		//Calculate proper count
		if(!spread)
			count = 1;
		else {
			if(count == 0)
				count = (int)stack->get_count();
			count -= skip;
		}

		//Apply sizing
		int i = (std::abs(offsetX) == tileX) ? 0 : count - 1;
		int j = (offsetY == tileY) ? 0 : count - 1;
		setSize(sf::Vector2i((tileX + std::abs(offsetX) * i) * scaleX, (tileY + offsetY * j) * scaleY));
		vertices.resize(4 * std::max(count, 1));
		setOrigin(0, 0);

		//Get first card
		Solisp::Card *card = stack->get_card();
		while(skip-- > 0 && card != NULL)
			card = card->get_next();

		if(card != NULL && count > 0) {
			// get the current tile
			int tileNumber = card->get_frame();
			if(card->is_hidden())
				tileNumber = 41;
			int tu = tileNumber % 14;
			int tv = tileNumber / 14;
			int shiftX = (offsetX < 0) ? tileX : 0;

			//Display on vertex
			sf::Vertex* quad = &vertices[(j + i) * 4];

			// define its 4 texture coordinates
			quad[0].texCoords = sf::Vector2f(tu * tileX, tv * tileY);
			quad[1].texCoords = sf::Vector2f((tu + 1) * tileX, tv * tileY);
			quad[2].texCoords = sf::Vector2f((tu + 1) * tileX, (tv + 1) * tileY);
			quad[3].texCoords = sf::Vector2f(tu * tileX, (tv + 1) * tileY);

			// define its 4 corners
			quad[0].position = sf::Vector2f(shiftX + i * offsetX, j * offsetY);
			quad[1].position = sf::Vector2f(shiftX + i * offsetX + tileX, j * offsetY);
			quad[2].position = sf::Vector2f(shiftX + i * offsetX + tileX, j * offsetY + tileY);
			quad[3].position = sf::Vector2f(shiftX + i * offsetX, j * offsetY + tileY);

			if(offsetX < 0)
				shiftX += tileX;

			while(card->get_next() != NULL && spread && std::abs(j + i) > 0) {
				(overlapY != 0) ? --j : --i;
				card = card->get_next();

				// get the current tile
				tileNumber = card->get_frame();
				if(card->is_hidden())
					tileNumber = 41;
				tu = tileNumber % 14;
				tv = tileNumber / 14;

				if(offsetX < 0)
					tu += 1;

				//Display on vertex
				quad = &vertices[(j + i) * 4];

				// define its 4 texture coordinates
				quad[0].texCoords = sf::Vector2f(tu * tileX, tv * tileY);
				quad[1].texCoords = sf::Vector2f(tu * tileX + offsetX + overlapX, tv * tileY);
				quad[2].texCoords = sf::Vector2f(tu * tileX + offsetX + overlapX, tv * tileY + offsetY + overlapY);
				quad[3].texCoords = sf::Vector2f(tu * tileX, tv * tileY + offsetY + overlapY);

				// define its 4 corners
				quad[0].position = sf::Vector2f(shiftX + i * offsetX, j * offsetY);
				quad[1].position = sf::Vector2f(shiftX + (i + 1) * offsetX + overlapX, j * offsetY);
				quad[2].position = sf::Vector2f(shiftX + (i + 1) * offsetX + overlapX, (j + 1) * offsetY + overlapY);
				quad[3].position = sf::Vector2f(shiftX + i * offsetX, (j + 1) * offsetY + overlapY);
			}
	    } else {
			vertices[0].position = sf::Vector2f(0, 0);
			vertices[1].position = sf::Vector2f(0, 0);
			vertices[2].position = sf::Vector2f(0, 0);
			vertices[3].position = sf::Vector2f(0, 0);
	    }

		//Draw to buffer
		/*buffer->clear(sf::Color::Transparent);
		buffer->draw(vertices, sf::RenderStates(&cardset));
		buffer->display();
		setTexture(buffer->getTexture());*/
	}

	unc getIndex() {
		return index;
	}

	sf::Vector2f getOffset(int count) {
		if(!spread)
			count = 0;
		if(stack->get_tag(SPREAD_REVERSE))
			return sf::Vector2f(
				(offsetX % tileX) * count * scaleX + tileX,
				(offsetY % tileY) * count * scaleY);
		return sf::Vector2f(
				(offsetX % tileX) * count * scaleX,
				(offsetY % tileY) * count * scaleY);
	}

	int checkOffset(sf::Vector2f pos) {
		if(!spread)
			return 0;
		int count = std::max((int)stack->get_count() - 1, 0);
		if(overlapY != 0)
			return bet(0, pos.y / (offsetY * scaleY), count);
		if(stack->get_tag(SPREAD_REVERSE))
			return bet(0, count - 1 + (pos.x - tileX - offsetX) / (offsetX * scaleX), count);
		return bet(0, pos.x / (offsetX * scaleX), count);
	}

	int bet(int min, int value, int max) {
		if(value < min)
			return min;
		if(value > max)
			return max;
		return value;
	}

	sf::Vector2f getCardSize() const {
		return sf::Vector2f(tileX * scaleX, tileY * scaleY);
	}
};