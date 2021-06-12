#include "ofApp.h"	

//--------------------------------------------------------------
Actor::Actor(vector<glm::vec3>& location_list, vector<vector<int>>& next_index_list, vector<int>& destination_list) {

	this->select_index = ofRandom(location_list.size());
	while (true) {

		auto itr = find(destination_list.begin(), destination_list.end(), this->select_index);
		if (itr == destination_list.end()) {

			destination_list.push_back(this->select_index);
			break;
		}

		this->select_index = (this->select_index + 1) % location_list.size();
	}

	this->next_index = this->select_index;
}

//--------------------------------------------------------------
void Actor::update(const int& frame_span, vector<glm::vec3>& location_list, vector<vector<int>>& next_index_list, vector<int>& destination_list) {

	if (ofGetFrameNum() % frame_span == 0) {

		auto tmp_index = this->select_index;
		this->select_index = this->next_index;
		int retry = next_index_list[this->select_index].size();
		this->next_index = next_index_list[this->select_index][(int)ofRandom(next_index_list[this->select_index].size())];
		while (--retry > 0) {

			auto destination_itr = find(destination_list.begin(), destination_list.end(), this->next_index);
			if (destination_itr == destination_list.end()) {

				if (tmp_index != this->next_index) {

					destination_list.push_back(this->next_index);
					break;
				}
			}

			this->next_index = next_index_list[this->select_index][(this->next_index + 1) % next_index_list[this->select_index].size()];
		}
		if (retry <= 0) {

			destination_list.push_back(this->select_index);
			this->next_index = this->select_index;
		}
	}

	auto param = ofGetFrameNum() % frame_span;
	auto distance = location_list[this->next_index] - location_list[this->select_index];
	this->location = location_list[this->select_index] + distance / frame_span * param;

	this->log.push_front(this->location);
	while (this->log.size() > 60) { this->log.pop_back(); }
}

//--------------------------------------------------------------
glm::vec3 Actor::getLocation() {

	return this->location;
}

//--------------------------------------------------------------
glm::vec3 Actor::getLocation(int i) {

	return i > 0 && i < this->log.size() ? this->log[i] : glm::vec3();
}

//--------------------------------------------------------------
deque<glm::vec3> Actor::getLog() {

	return this->log;
}

//--------------------------------------------------------------
void Actor::setColor(ofColor value) {

	this->color = value;
}

//--------------------------------------------------------------
ofColor Actor::getColor() {

	return this->color;
}


//--------------------------------------------------------------
void ofApp::setup() {

	ofSetFrameRate(60);
	ofSetWindowTitle("openFrameworks");

	ofBackground(0);
	ofSetLineWidth(1.5);
	ofEnableDepthTest();

	int span = 80;
	for (int x = -200; x <= 200; x += span) {

		for (int y = -200; y <= 200; y += span) {

			for (int z = -200; z <= 200; z += span) {

				this->location_list.push_back(glm::vec3(x, y, z));
			}
		}
	}

	for (auto& location : this->location_list) {

		vector<int> next_index = vector<int>();
		int index = -1;
		for (auto& other : this->location_list) {

			index++;
			if (location == other) { continue; }

			float distance = glm::distance(location, other);
			if (distance <= span) {

				next_index.push_back(index);
			}
		}

		this->next_index_list.push_back(next_index);
	}

	ofColor color;
	vector<ofColor> base_color_list;
	vector<int> hex_list = { 0xef476f, 0xffd166, 0x06d6a0, 0x118ab2, 0xefefef };
	for (auto hex : hex_list) {

		color.setHex(hex);
		base_color_list.push_back(color);
	}

	for (int i = 0; i < 120; i++) {

		this->actor_list.push_back(make_unique<Actor>(this->location_list, this->next_index_list, this->destination_list));
		this->actor_list.back()->setColor(base_color_list[(int)ofRandom(base_color_list.size())]);
	}
}

//--------------------------------------------------------------
void ofApp::update() {

	int frame_span = 20;
	int prev_index_size = 0;

	if (ofGetFrameNum() % frame_span == 0) {

		prev_index_size = this->destination_list.size();
	}

	for (auto& actor : this->actor_list) {

		actor->update(frame_span, this->location_list, this->next_index_list, this->destination_list);
	}

	if (prev_index_size != 0) {

		this->destination_list.erase(this->destination_list.begin(), this->destination_list.begin() + prev_index_size);
	}
}

//--------------------------------------------------------------
void ofApp::draw() {

	this->cam.begin();

	ofRotateY(ofGetFrameNum() * 0.5);
	ofRotateX(ofGetFrameNum() * 0.38);

	
	for (auto& actor : this->actor_list) {

		ofFill();
		ofSetColor(39);
		ofDrawBox(actor->getLocation(), 70);

		ofNoFill();
		ofSetColor(actor->getColor());
		ofDrawBox(actor->getLocation(), 71);
		ofDrawBox(actor->getLocation(20), 71);
	}

	this->cam.end();
}


//--------------------------------------------------------------
int main() {

	ofSetupOpenGL(720, 720, OF_WINDOW);
	ofRunApp(new ofApp());
}