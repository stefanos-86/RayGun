#include "pch.h"

#include "World.h"

#include <istream>
#include <sstream>
#include <stdexcept>

#include <iostream>

namespace rc {
	template <typename READ_TYPE>
	static READ_TYPE read_token(const std::string& expected_token, std::istream& serialized_world) {
		std::string name;
		READ_TYPE value;

		if (serialized_world.bad())
			throw std::runtime_error("Bad stream.");

		serialized_world >> name >> value;

		std::cout << "-" << name << "-" << value << std::endl;

		if (name != expected_token) {
			std::stringstream error_message;
			error_message << "Token " << expected_token << " not found. Got [" << name << "].";
			throw std::runtime_error(error_message.str());
		}
	
		return value;
	}

	/** Specialization for short ints, that are taken as chars from the stream. The value would get 
	an ASCII code inside. Read as int, chech range and cast. */
	template <>
	static uint8_t read_token<uint8_t>(const std::string& expected_token, std::istream& serialized_world) {
		const int value = read_token<int>(expected_token, serialized_world);
		if (value < 0 || value > 255)
			throw std::runtime_error("Int outside unsigned 8 bit range.");
		return (uint8_t)value;
	}


	World World::load(std::istream& serialized_world)
	{
		if (!serialized_world.good())
			throw std::runtime_error("Bad before beginning. Empty?");

		const uint8_t x = read_token<uint8_t>("x", serialized_world);
		const uint8_t z = read_token<uint8_t>("z", serialized_world);

		char cell;
		if (!serialized_world.get(cell)) // Skip the \n after the z.
			throw std::runtime_error("No grid data");


		Grid g(x, z, 64);  // TODO: do I make the cell size a parameter?? I don't think the rest of the code is ready for this.
		Enemies e;
		uint8_t sprite_id = 0;

		uint8_t column_x = 0;
		uint8_t row_z = 0;
		const uint8_t half_cell = g.cell_size / 2;  // TODO: move in the grid (center_of(cell)?).
		while (row_z < g.z_size && serialized_world.get(cell)) {
			switch (cell)
			{
			case '#':
				g.build_wall(column_x, row_z);
				[[fallthrough]];
			case '.':
				++column_x;
				break;
			case 'E':
				e.sprites.emplace_back(g.cell_size * column_x + half_cell, g.cell_size * row_z + half_cell, 64, sprite_id++);  // TODO Again the damn size hardcode. And the id computed outside the Sprite class.
				++column_x;
				break;
			case '\n':
				column_x = 0;
				++row_z;
				break;
			default:
				std::stringstream error;
				error << "Invalid char [" << cell << "] at " << (int) column_x << ", " << (int) row_z;
				throw std::runtime_error(error.str());
			}
		}

		const float player_x = read_token<float>("player_start_x", serialized_world);  // TODO this is silly, the player start could be on the map.
		const float player_z = read_token<float>("player_start_z", serialized_world);
		const float player_orientation = read_token<float>("player_start_orientation_rad", serialized_world);
		const uint8_t ammo = read_token<uint8_t>("player_ammo", serialized_world);

		Player p{player_x, player_z, player_orientation, ammo};  // Not set: the kills, that begin at 0.
		
		return World{g, p, e};
	}

}