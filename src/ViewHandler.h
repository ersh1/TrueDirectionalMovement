#pragma once

class ViewHandler :
	public RE::BSTEventSink<RE::MenuOpenCloseEvent>
{
public:
	enum MenuType : std::uint8_t
	{
		kTargetLockMenu,
		kBossMenu
	};

	ViewHandler() = delete;
	ViewHandler(const ViewHandler&) = default;
	ViewHandler(ViewHandler&&) = default;

	ViewHandler(SKSE::stl::observer<RE::IMenu*> a_menu, MenuType a_menuType) :
		_menu(a_menu),
		_view(a_menu ? a_menu->uiMovie : nullptr),
		_menuType(a_menuType)
	{
		assert(_menu != nullptr);
		assert(_view != nullptr);

		Register();
	}

	~ViewHandler()
	{
		Unregister();
	}

	ViewHandler& operator=(const ViewHandler&) = default;
	ViewHandler& operator=(ViewHandler&&) = default;

protected:
	using EventResult = RE::BSEventNotifyControl;

	EventResult ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>*) override
	{
		auto intfcStr = RE::InterfaceStrings::GetSingleton();
		if (intfcStr &&
			a_event &&
			a_event->menuName == intfcStr->mapMenu /*||
			a_event->menuName == intfcStr->tweenMenu*/
		) {
			if (!a_event->opening) {
				Enable();
			} else {
				Disable();
			}
		}

		return EventResult::kContinue;
	}

private:
	void Register()
	{
		auto menuSrc = RE::UI::GetSingleton();
		if (menuSrc) {
			menuSrc->AddEventSink(this);
		}
	}

	void Unregister()
	{
		auto menuSrc = RE::UI::GetSingleton();
		if (menuSrc) {
			menuSrc->RemoveEventSink(this);
		}
	}

	void Enable()
	{
		RE::GPtr safety{ _menu };
		auto task = SKSE::GetTaskInterface();
		task->AddUITask([this, safety]() {
			if (_hideCount > 0) {
				_hideCount--;
				if (_hideCount == 0) {
					SetVisible(true);
				}	
			}
		});
	}

	void Disable()
	{
		RE::GPtr safety{ _menu };
		auto task = SKSE::GetTaskInterface();
		task->AddUITask([this, safety]() {
			_hideCount++;
			if (_hideCount > 0) {
				SetVisible(false);
			}
		});
	}

	void SetVisible(bool a_visible);

	void Close();

	SKSE::stl::observer<RE::IMenu*> _menu;
	RE::GPtr<RE::GFxMovieView> _view;
	MenuType _menuType;
	uint8_t _hideCount = 0;
};
