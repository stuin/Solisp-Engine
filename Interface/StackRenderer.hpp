#include <algorithm>

#include "main.h"
#include "Skyrmion/UpdateList.h"
#include "../Gameplay/game.h"

class StackRenderer : public Node {
public:
	Solisp::Stack *stack;
	bool spread;
	bool vspread;
	bool hspread;

	static sf::Texture cardset;
	static float cardScaling;

private:
	unc index;
	sf::VertexArray vertices;

	//Card image sizes
	const int tileX = 206;
	const int tileY = 284;
	const int baseGapX = 106;
	const int baseGapY = 76;

	//Card rendering adjusters
	int offsetX = tileX;
	int offsetY = tileY / 6.5;
	int overlapX = 0;
	int overlapY = 10;
	int gapX;
	int gapY;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const {
		states.texture = &cardset;
		states.transform *= getTransform();
		target.draw(vertices, states);
	}

public:
	StackRenderer(Solisp::Stack *stack, unc index, Node *root, Layer layer=STACKS, float offsetDiv=4.5) : Node(layer, sf::Vector2i(1, 1), false, root) {
		this->stack = stack;
		this->index = index;

		setGameSize(
			(stack->x + stack->width) * baseGapX + 10, 
			(stack->y + stack->height) * baseGapY + 10);
		vertices.setPrimitiveType(sf::Quads);
		UpdateList::addNode(this);

		if(stack->get_tag(HIDDEN))
			setHidden(true);

		//Edit offset values for spreading
		spread = vspread = stack->get_tag(SPREAD);
		if(stack->get_tag(SPREAD_HORIZONTAL)) {
			hspread = true;
			vspread = false;
			offsetX /= offsetDiv;
			overlapX = 10;
			offsetY = tileY;
			overlapY = 0;

			if(stack->get_tag(SPREAD_REVERSE)) {
				offsetX *= -1;
				overlapX *= -1;
				setPosition((stack->x + 4) * gapX + 65, stack->y * gapY + 30);
			}
		} else if(!spread) {
			offsetY = tileY;
			overlapY = 0;
		}
	}

	void reload(int count = 0, int skip = 0) {
		if(stack->get_tag(HIDDEN))
			return;

		//Calculate proper count
		if(!spread)
			count = 1;
		else {
			if(count == 0)
				count = (int)stack->get_count();
			count -= skip;
		}

		//Recheck scaling
		gapX = baseGapX * cardScaling;
		gapY = baseGapY * cardScaling;
		setScale(cardScaling, cardScaling);
		setPosition(stack->x * gapX, stack->y * gapY);

		//Apply card count sizing
		int i = (std::abs(offsetX) == tileX) ? 0 : count - 1;
		int j = (offsetY == tileY) ? 0 : count - 1;
		setSize(sf::Vector2i((tileX + std::abs(offsetX) * i) * cardScaling, (tileY + offsetY * j) * cardScaling));
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

			//Check reversed
			int shiftX = (offsetX < 0) ? tileX - offsetX : 0;

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

				//Check reversed
				if(offsetX < 0)
					tu += 1;

				//Display on vertex
				quad = &vertices[(j + i) * 4];

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
			}
	    } else {
			vertices[0].position = sf::Vector2f(0, 0);
			vertices[1].position = sf::Vector2f(0, 0);
			vertices[2].position = sf::Vector2f(0, 0);
			vertices[3].position = sf::Vector2f(0, 0);
	    }
	}

	void recieveSignal(int id) {
		if(id == RELOADCARDS)
			reload();
	}

	sf::Vector2f getOffset(int count) {
		if(!spread)
			count = 0;
		int shiftX = 0;
		if(stack->get_tag(SPREAD_REVERSE))
			shiftX = tileX - (offsetX * 4) - 3;

		return sf::Vector2f(
				(offsetX % tileX) * count * cardScaling + shiftX,
				(offsetY % tileY) * count * cardScaling);
	}

	int checkOffset(sf::Vector2f pos) {
		if(!spread)
			return 0;
		int count = std::max((int)stack->get_count() - 1, 0);
		pos += sf::Vector2f(1, 1);

		//Vertical
		if(overlapY != 0)
			return bet(0, pos.y / (offsetY * cardScaling), count);

		//Horizontal reversed
		if(stack->get_tag(SPREAD_REVERSE))
			return bet(0, count - 1 +
				(pos.x - (tileX - offsetX) * cardScaling) / (offsetX * cardScaling), count);

		//Horizontal
		return bet(0, pos.x / (offsetX * cardScaling), count);
	}

	unc getIndex() {
		return index;
	}

	sf::Vector2f getCardSize() const {
		return sf::Vector2f(tileX * cardScaling, tileY * cardScaling);
	}

	sf::Vector2f getCardOffset() {
		return sf::Vector2f(offsetX * cardScaling, offsetY * cardScaling);
	}

	sf::Vector2f getCardGap() {
		return sf::Vector2f(gapX, gapY);
	}
};